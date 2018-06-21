//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
//EnemyWave
//
//PURPOSE:
//This class stores groups of enemies to signify one full wave in a level
//
//CREDIT:
//Nick
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*

#include "EnemyWave.h"
#include "EventManager.h"

//For this constructor, we plan on adding the enemy waves later, so we state that the wave is not
// finished
EnemyWave::EnemyWave(bool p_bNight)
    :m_bNightWave(p_bNight)
{
    m_bWaveIsFinished = false;
    m_iGroupIndex = 0;
}

//For the assignment constructor, we get the list and we get the first element out of the list
EnemyWave::EnemyWave(std::vector<EnemyGroup*> p_vGroups)
{
    m_vGroups = p_vGroups;
    m_iGroupIndex = 0;

    m_bWaveIsFinished = false;
}

EnemyWave::~EnemyWave()
{
	if(!m_vGroups.empty())
	{
		m_vGroups.clear();
	}
}

void EnemyWave::Reset()
{
    m_bWaveIsFinished = false;

    for(int j = 0; j < m_vGroups.size(); j++)
    {
        m_vGroups[j]->Reset();
    }
}

void EnemyWave::Update(Ogre::FrameEvent evt)
{
    if(m_iGroupIndex < m_vGroups.size())
    {
        //If our current group isn't finished, we update it
        if(!m_vGroups[m_iGroupIndex]->IsFinished())
        {
            m_vGroups[m_iGroupIndex]->Update(evt);
        }
        else
        {
            m_iGroupIndex++;
        }
    }
    //If not, we're done
    else if (EnemyManager::Instance()->Size() == 0)
    {
        m_bWaveIsFinished = true;
    }
}

