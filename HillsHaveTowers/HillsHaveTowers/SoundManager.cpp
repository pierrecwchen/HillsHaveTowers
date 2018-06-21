#include "SoundManager.h"

FMOD::System* SoundManager::m_sSystem;
bool SoundManager::m_bWasInitialized = false;
bool SoundManager::m_bMuteSounds = false;
bool SoundManager::m_bMuteMusic = false;
std::map<std::string, FMOD::Sound*> SoundManager::m_mSoundMap;
std::vector<FMOD::Channel*> SoundManager::m_vPlayingChannels;
float SoundManager::m_fVolume = 1.0f;
FMOD::Channel* SoundManager::m_cMusic;


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        MessageBox( NULL, FMOD_ErrorString(result), "An FMOD exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
        MessageBox( NULL, FMOD_ErrorString(result), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
        exit(-1);
    }
}

bool SoundManager::Init()
{
    FMOD_RESULT result;
    unsigned int version;
    int numdrivers;
    FMOD_SPEAKERMODE speakermode;
    FMOD_CAPS caps;
    char name[256];

    /*
    Create a System object and initialize.
    */
    result = FMOD::System_Create(&m_sSystem);
    ERRCHECK(result);

    result = m_sSystem->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error! You are using an old version of FMOD %08x. This program requires %08x\n",
            version, FMOD_VERSION);
        return 0;
    }

    result = m_sSystem->getNumDrivers(&numdrivers);
    ERRCHECK(result);

    if (numdrivers == 0)
    {
        result = m_sSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
        ERRCHECK(result);
    }
    else
    {
        result = m_sSystem->getDriverCaps(0, &caps, 0, &speakermode);
        ERRCHECK(result);

        /*
        Set the user selected speaker mode.
        */
        result = m_sSystem->setSpeakerMode(speakermode);
        ERRCHECK(result);

        if (caps & FMOD_CAPS_HARDWARE_EMULATED)
        {
            /*
            The user has the 'Acceleration' slider set to off! This is really bad
            for latency! You might want to warn the user about this.
            */
            result = m_sSystem->setDSPBufferSize(1024, 10);
            ERRCHECK(result);
        }

        result = m_sSystem->getDriverInfo(0, name, 256, 0);
        ERRCHECK(result);

        if (strstr(name, "SigmaTel"))
        {
            /*
            Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
            PCM floating point output seems to solve it.
            */
            result = m_sSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0,
                FMOD_DSP_RESAMPLER_LINEAR);
            ERRCHECK(result);
        }
    }

    result = m_sSystem->init(MAX_CHANNELS, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);

    if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
    {
        /*
        Ok, the speaker mode selected isn't supported by this soundcard. Switch it
        back to stereo...
        */
        result = m_sSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
        ERRCHECK(result);

        /*
        ... and re-init.
        */
        result = m_sSystem->init(MAX_CHANNELS, FMOD_INIT_NORMAL, 0);
    }
    ERRCHECK(result);

    m_sSystem->set3DSettings(1.0, 1.0, 0.01);

    

    FMOD::Debug_SetLevel(FMOD_DEBUG_LEVEL_NONE);

    InitMenuSounds();

    m_bWasInitialized = true;

    return true;
}

void SoundManager::PositionListener(Ogre::Vector3 p_v3Camera, Ogre::Vector3 p_v3Direction)
{
    FMOD_VECTOR camera, direction, up, vel;

    camera.x = p_v3Camera.x;
    camera.y = p_v3Camera.y;
    camera.z = p_v3Camera.z;

    direction.x = p_v3Direction.x;
    direction.y = p_v3Direction.y;
    direction.z = p_v3Direction.z;

    up.x = 0;
    up.y = 1;
    up.z = 0;

    vel.x = 0;
    vel.y = 0;
    vel.z = 0;

    m_sSystem->set3DListenerAttributes(0, &camera, &vel, &direction, &up);
    m_sSystem->update();
}

void SoundManager::InitMenuSounds()
{
    std::ifstream file;
    std::string line;

    file.open("../Media/sound/MenuSounds.txt", std::ios::in);
    getline(file, line);

    FMOD::Sound* tempSound;
    FMOD_RESULT result;

    while(line.at(0) != '#')
    {
        std::string fullPath = "../Media/sound/";

        fullPath.append(line);

        result = m_sSystem->createSound(fullPath.c_str(), FMOD_DEFAULT | FMOD_2D | FMOD_LOOP_OFF, 0, &tempSound);
        ERRCHECK(result);

        m_mSoundMap[line] = tempSound;

        getline(file, line);
    }

    file.close();

    file.open("../Media/sound/MenuLoops.txt", std::ios::in);
    getline(file, line);

    while(line.at(0) != '#')
    {
        std::string fullPath = "../Media/sound/";

        fullPath.append(line);

        result = m_sSystem->createSound(fullPath.c_str(), FMOD_DEFAULT | FMOD_2D | FMOD_LOOP_NORMAL, 0, &tempSound);
        ERRCHECK(result);

        m_mSoundMap[line] = tempSound;

        getline(file, line);
    }

    file.close();
}

bool SoundManager::InitGameSounds()
{
    std::ifstream file;
    std::string line;

    file.open("../Media/sound/3DGameSounds.txt", std::ios::in);
    getline(file, line);

    FMOD::Sound* tempSound;
    FMOD_RESULT result;

    while(line.at(0) != '#')
    {
        std::string fullPath = "../Media/sound/";

        fullPath.append(line);

        result = m_sSystem->createSound(fullPath.c_str(), FMOD_DEFAULT | FMOD_3D_INVERSEROLLOFF | FMOD_LOOP_OFF, 0, &tempSound);
        ERRCHECK(result);

        m_mSoundMap[line] = tempSound;

        getline(file, line);
    }

    file.close();

    file.open("../Media/sound/3DGameLoops.txt", std::ios::in);
    getline(file, line);

    while(line.at(0) != '#')
    {
        std::string fullPath = "../Media/sound/";

        fullPath.append(line);

        result = m_sSystem->createSound(fullPath.c_str(), FMOD_DEFAULT | FMOD_3D_INVERSEROLLOFF | FMOD_LOOP_NORMAL, 0, &tempSound);
        ERRCHECK(result);

        m_mSoundMap[line] = tempSound;

        getline(file, line);
    }

    file.close();

    return true;
}

void SoundManager::PlaySound(std::string p_cFileName)
{
    if(m_bWasInitialized)
    {
        FMOD::Channel* channel;
        m_sSystem->playSound(FMOD_CHANNEL_FREE, m_mSoundMap[p_cFileName], false, &channel);

        FMOD_RESULT result = channel->setVolume(m_fVolume);
        ERRCHECK(result);

        channel->setMute(m_bMuteSounds);
    }
}

void SoundManager::PlayMusic(std::string p_cFileName)
{
    if(m_bWasInitialized)
    {
        FMOD_RESULT result = m_sSystem->playSound(FMOD_CHANNEL_REUSE, m_mSoundMap[p_cFileName], false, &m_cMusic);

        result = m_cMusic->setVolume(m_fVolume);
        ERRCHECK(result);

        m_cMusic->setMute(m_bMuteMusic);
    }
}

int SoundManager::Play2DSound(std::string p_cFileName, float p_fPan, float p_fVolume)
{
    if(m_bWasInitialized)
    {
        FMOD::Channel* channel;
        m_sSystem->playSound(FMOD_CHANNEL_FREE, m_mSoundMap[p_cFileName], false, &channel);

        FMOD_RESULT result = channel->setVolume(m_fVolume);
        ERRCHECK(result);

        result = channel->setPan(p_fPan);
        ERRCHECK(result);

        m_sSystem->update();

        channel->setMute(m_bMuteSounds);

        int index;
        channel->getIndex(&index);

        return index;
    }
}

int SoundManager::Play3DSound(std::string p_cFileName, Ogre::Vector3 p_v3Location, float p_fVolume)
{
    if(m_bWasInitialized)
    {
        FMOD_VECTOR location;

        location.x = p_v3Location.x;
        location.y = p_v3Location.y;
        location.z = p_v3Location.z;

        FMOD::Channel* channel;
        FMOD_RESULT result = m_sSystem->playSound(FMOD_CHANNEL_FREE, m_mSoundMap[p_cFileName], false, &channel);
        ERRCHECK(result);

        result = channel->set3DAttributes(&location, NULL);
        ERRCHECK(result);

        result = channel->setVolume(m_fVolume * p_fVolume);
        ERRCHECK(result);

        channel->setMute(m_bMuteSounds);

        m_sSystem->update();

        int index;
        channel->getIndex(&index);

        return index;

       
    }

    return -1;
}

int SoundManager::Play3DPitchedSound(std::string p_cFileName, Ogre::Vector3 p_v3Location, float p_fPitchVariation, float p_fVolume, float p_fPitchModifier)
{
    if(m_bWasInitialized)
    {
        FMOD_VECTOR location;

        location.x = p_v3Location.x;
        location.y = p_v3Location.y;
        location.z = p_v3Location.z;

        FMOD::Channel* channel;
        FMOD_RESULT result = m_sSystem->playSound(FMOD_CHANNEL_FREE, m_mSoundMap[p_cFileName], false, &channel);
        ERRCHECK(result);

        result = channel->set3DAttributes(&location, NULL);
        ERRCHECK(result);

        result = channel->setVolume(m_fVolume * p_fVolume);
        ERRCHECK(result);

        float pitchModifier = p_fPitchModifier + ((float)rand() / (float)RAND_MAX * 2 * p_fPitchVariation - p_fPitchVariation);
        float currentFrequency;

        result = channel->getFrequency(&currentFrequency);
        ERRCHECK(result);

        result = channel->setFrequency(currentFrequency * pitchModifier);
        ERRCHECK(result);

        channel->setMute(m_bMuteSounds);

        m_sSystem->update();

        int index;
        channel->getIndex(&index);

        return index;


    }

    return -1;
}

int SoundManager::Play3DLoopedSound(std::string p_cFileName, Ogre::Vector3 p_v3Location, float p_fVolume)
{
    if(m_bWasInitialized)
    {
        FMOD_VECTOR location;

        location.x = p_v3Location.x;
        location.y = p_v3Location.y;
        location.z = p_v3Location.z;

        FMOD::Channel* channel;
        FMOD_RESULT result = m_sSystem->playSound(FMOD_CHANNEL_FREE, m_mSoundMap[p_cFileName], false, &channel);
        ERRCHECK(result);

        result = channel->set3DAttributes(&location, NULL);
        ERRCHECK(result);

        result = channel->setLoopCount(-1);
        ERRCHECK(result);

        result = channel->setVolume(m_fVolume * p_fVolume);
        ERRCHECK(result);

        channel->setMute(m_bMuteSounds);

        m_sSystem->update();

        int index;
        channel->getIndex(&index);

        return index;
    }

    return -1;
}

void SoundManager::StopSoundAtChannel(int p_iChannelIndex)
{
    if(m_bWasInitialized)
    {
        FMOD::Channel* tempChannel;

        m_sSystem->getChannel(p_iChannelIndex, &tempChannel);

        tempChannel->stop();
    }
}

float SoundManager::GetLength(std::string p_cFileName)
{
    if(m_bWasInitialized)
    {
        unsigned int soundLength;
        m_mSoundMap[p_cFileName]->getLength(&soundLength, FMOD_TIMEUNIT_MS);

        float resultLength = (float)soundLength / 1000.0f;

        return resultLength;
    }

    return 1.0f;
}

void SoundManager::SetVolumeAtChannel(float p_fVolume, int p_iIndex)
{
    if(m_bWasInitialized)
    {
        FMOD::Channel* tempChannel;

        m_sSystem->getChannel(p_iIndex, &tempChannel);

        tempChannel->setVolume(m_fVolume * p_fVolume);
    }
}

void SoundManager::GetAllPlayingChannels()
{
    FMOD::Channel* tempChannel;
    int channelCount;
    FMOD_RESULT result;

    m_sSystem->getSoftwareChannels(&channelCount);

    int musicIndex;
    m_cMusic->getIndex(&musicIndex);

    for(int j = MAX_CHANNELS - 1; j > 0; j--)
    {
        if(j != musicIndex)
        {
            result = m_sSystem->getChannel(j, &tempChannel);
            //ERRCHECK(result);

            bool playing;
            result = tempChannel->isPlaying(&playing);
            //ERRCHECK(result);

            if(playing)
            {
                m_vPlayingChannels.push_back(tempChannel);
            }
        }
    }
}

void SoundManager::PauseAllSounds()
{
    GetAllPlayingChannels();

    for(int j = 0; j < m_vPlayingChannels.size(); j++)
    {
        m_vPlayingChannels[j]->setPaused(true);     
    }
    
}

void SoundManager::ResumeAllSounds()
{
    for(int j = 0; j < m_vPlayingChannels.size(); j++)
    {
        m_vPlayingChannels[j]->setPaused(false);
    }

    m_vPlayingChannels.clear();
}

void SoundManager::StopAllSounds()
{
    if(m_vPlayingChannels.empty())
    {
        GetAllPlayingChannels();
    }

    for(int j = 0; j < m_vPlayingChannels.size(); j++)
    {
        m_vPlayingChannels[j]->stop();
    }

    m_vPlayingChannels.clear();
}

void SoundManager::ToggleMuteAllSounds()
{
    m_bMuteSounds = !m_bMuteSounds;

    if(m_vPlayingChannels.empty())
    {
        GetAllPlayingChannels();
    }

    for(int j = 0; j < m_vPlayingChannels.size(); j++)
    {
        m_vPlayingChannels[j]->setMute(m_bMuteSounds);
    }

    m_vPlayingChannels.clear();
}


