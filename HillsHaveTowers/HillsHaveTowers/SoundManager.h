#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "fmod.hpp"
#include "fmod_errors.h"

class SoundManager
{
public:

    static bool Init();
    static bool InitGameSounds();

    static void PositionListener(Ogre::Vector3 p_v3Camera, Ogre::Vector3 p_v3Direction);

    static void PlaySound(std::string p_cFileName);
    static int Play2DSound(std::string p_cFileName, float p_fPan = 0.0f, float p_fVolume = 1.0f);
    static int Play3DSound(std::string p_cFileName, Ogre::Vector3 p_v3Location, float p_fVolume = 1.0f); 
    static int Play3DPitchedSound(std::string p_cFileName, Ogre::Vector3 p_v3Location, float p_fPitchVariation, float p_fVolume = 1.0f, float p_fPitchModifier = 1.0f);
    static int Play3DLoopedSound(std::string p_cFileName, Ogre::Vector3 p_v3Location, float p_fVolume = 1.0f);

    static void PlayMusic(std::string p_cFileName);
    static void PauseMusic() { m_cMusic->setPaused(true); }
    static void ResumeMusic() { m_cMusic->setPaused(false); }
    static void ToggleMuteMusic() { m_bMuteMusic = !m_bMuteMusic; m_cMusic->setMute(m_bMuteMusic); }
    static void LowerMusicVolume() { m_cMusic->setVolume(0.4f * m_fVolume); }
    static void RaiseMusicVolume() { m_cMusic->setVolume(m_fVolume); }
    static void StopMusic() { m_cMusic->stop(); }

    static void StopSoundAtChannel(int p_iChannelIndex);

    static float GetLength(std::string p_cFileName); 

    static void SetVolume(float p_fVolume) { m_fVolume = p_fVolume; }
    static void SetVolumeAtChannel(float p_fVolume, int p_iIndex);

    static void PauseAllSounds();
    static void ResumeAllSounds();
    static void StopAllSounds();
    static void ToggleMuteAllSounds();

private:
    static void InitMenuSounds();

    static const int MAX_CHANNELS = 100;

    static void GetAllPlayingChannels();

    static FMOD::System* m_sSystem;
    static FMOD::Channel* m_cMusic;

    static bool m_bWasInitialized, m_bMuteSounds, m_bMuteMusic;

    static float m_fMinDistance, m_fMaxDistance, m_fVolume;

    static std::map<std::string, FMOD::Sound*> m_mSoundMap;
    static std::vector<FMOD::Channel*> m_vPlayingChannels;
};

#endif