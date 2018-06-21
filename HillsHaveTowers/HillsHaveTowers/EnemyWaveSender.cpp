//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
//EnemyWaveSender
//
//PURPOSE:
//This stores all the waves for an entire level
//
//CREDIT:
//Nick
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*

#include "EnemyWaveSender.h"
#include "GameDataManager.h"

//The structure in this class is extremely similar to EnemyWave's structure
EnemyWaveSender::EnemyWaveSender():	m_bAnimStart(true)
{
    m_bAllEnemiesSent = false;
    m_iWaveIndex = 0;
}

EnemyWaveSender::EnemyWaveSender(std::vector<EnemyWave*> p_vWaves):	m_bAnimStart(true)
{
    m_vWaves = p_vWaves;
    m_iWaveIndex = 0;

    m_bAllEnemiesSent = false;
}

EnemyWaveSender::~EnemyWaveSender()
{
	while(!m_vWaves.empty())
	{
		m_vWaves.clear();
	}
}

void EnemyWaveSender::ResetTo(int p_iStartingWave)
{
    m_bAllEnemiesSent = false;

	int currentGroup = 0;

	for(int i = 0; i < p_iStartingWave; ++i)
	{
		currentGroup += m_vWaves[i]->GetGroupNum();
	}

	EnemyBar::GetSingletonPtr()->StartAnimWithWave(currentGroup, m_vWaves[p_iStartingWave]->GetGroupNum());

    for(int j = p_iStartingWave; j < m_vWaves.size(); j++)
    {
        m_vWaves[j]->Reset();
    }
}

void EnemyWaveSender::Update(Ogre::FrameEvent evt)
{
    if(m_iWaveIndex < m_vWaves.size())
    {
        if(m_bAnimStart)
        {
            //SoundManager::Play2DSound("waveStart.wav", 0.0f, 0.75f);
            EnemyBar::GetSingletonPtr()->SendWave(m_vWaves[m_iWaveIndex]->GetGroupNum());
            m_bAnimStart = false;
        }

        if(!m_vWaves[m_iWaveIndex]->IsFinished())
        {
            m_vWaves[m_iWaveIndex]->Update(evt);
        }
        else
        {
            m_iWaveIndex++;
            Hills::EventManager::AddEvent(Hills::GameEvent(Hills::GE_DayNightChange,
                                                           m_vWaves[m_iWaveIndex]->IsNight()));
            SoundManager::Play2DSound("waveEnd.wav", 0.0f, 0.75f);
			GameDataManager::GetSingletonPtr()->GetCurrentGameData()->SetCurrentWave(m_iWaveIndex);
			m_bAnimStart = true;
        }
    }
    else
    {
        m_bAllEnemiesSent = true;
    }
}