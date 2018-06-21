//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
//EnemyGroup
//
//PURPOSE:
//The lowest level implementation of enemies, a single group of enemies that gets spawned together.
// For example, a group of 3 soldiers will get spawned in 10 seconds.
//This class is the one that actually makes the call to enemy manager
//
//CREDIT:
//Nick
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*

#include "EnemyGroup.h"
#include "EnemyBar.h"
#include "EventManager.h"

//Empty constructor intitializes everything to zero which in turn, means no more enemies need to be spawned
EnemyGroup::EnemyGroup()
{
    m_eeEnemyType = SoldierEnemy;
    m_iCount = 0;
    m_fSendTime = 0;
    m_bHasPriest = false;
    m_fTimeWaited = 0;
    m_bAllEnemiesSent = true;
    m_bFirstHalfSent = true;
	//m_bAnimStart = false;
}

//Assignment constructor
EnemyGroup::EnemyGroup(Enemies p_eeEnemyType, int p_iCount, float p_fLaunchTime, bool p_bWithPriest)
{
    m_eeEnemyType = p_eeEnemyType;
    m_iCount = p_iCount;
    m_fSendTime = p_fLaunchTime;
    m_bHasPriest = p_bWithPriest;
    m_fTimeWaited = 0;
    m_bAllEnemiesSent = false;
    m_bFirstHalfSent = false;
	//m_bAnimStart = false;
}

void EnemyGroup::Reset()
{
    //m_bAnimStart = false;
    m_bAllEnemiesSent = false;
    m_bFirstHalfSent = false; 
    m_fTimeWaited = 0;
}

//Update waits until it's time to spawn enemies and then when it is, it spawns them in a staggered
// format
void EnemyGroup::Update(Ogre::FrameEvent evt)
{
	m_fTimeWaited += evt.timeSinceLastFrame * BaseApplication::GetGameSpeed();

    //If we're ready to spawn enemies
    if(m_fTimeWaited >= m_fSendTime)
    {
        if(m_eeEnemyType == ET_DEMON)
        {
            Hills::EventManager::AddEvent(Hills::GameEvent(Hills::GE_BossIncoming));
        }

        if(m_iCount == 5)
        {
            m_bAllEnemiesSent = SpawnGroup(evt);
        }
        else
        {
            m_bAllEnemiesSent = SpawnGroup();
        }
    }
}

bool EnemyGroup::SpawnGroup()
{
   if(m_bHasPriest)
   {
       m_iCount++;
   }

   float size = Tile::GetDefaultSize() / 2;

   if(m_iCount == 1)
   {
       EnemyManager::Instance()->Add(m_eeEnemyType, 0, 0);
   }
   else if(m_iCount == 2)
   {
       EnemyManager::Instance()->Add(m_eeEnemyType, size * 0.65, 0);
       EnemyManager::Instance()->Add(m_eeEnemyType, size * -0.65, 0);
   }
   else if(m_iCount == 3)
   {
       EnemyManager::Instance()->Add(m_eeEnemyType, size * 0.65, size * 0.4);
       EnemyManager::Instance()->Add(m_eeEnemyType, size * -0.65, size * 0.4);
       
       if(m_bHasPriest)
       {
           EnemyManager::Instance()->Add(PriestEnemy, 0, size * -0.3);
       }
       else
       {
           EnemyManager::Instance()->Add(m_eeEnemyType, 0, size * -0.3);
       }
   }
   else if(m_iCount == 4)
   {
       if(m_bHasPriest)
       {
           EnemyManager::Instance()->Add(m_eeEnemyType, size * 0.65, size * 0.4);
           EnemyManager::Instance()->Add(m_eeEnemyType, size * -0.65, size * 0.4);
           EnemyManager::Instance()->Add(PriestEnemy, 0, 0);
           EnemyManager::Instance()->Add(m_eeEnemyType, 0, size * -0.7);
       }
       else
       {
           EnemyManager::Instance()->Add(m_eeEnemyType, size * 0.65, size * 0.4);
           EnemyManager::Instance()->Add(m_eeEnemyType, size * -0.65, size * 0.4);
           EnemyManager::Instance()->Add(m_eeEnemyType, size * 0.65, size * -0.4);
           EnemyManager::Instance()->Add(m_eeEnemyType, size * -0.65, size * -0.4);
       }
   }
   else if(m_iCount == 5)
   {
       if(m_bHasPriest)
       {
           EnemyManager::Instance()->Add(m_eeEnemyType, size * 0.85, size * 0.6);
           EnemyManager::Instance()->Add(m_eeEnemyType, size * -0.85, size * 0.6);
           EnemyManager::Instance()->Add(m_eeEnemyType, size * 0.85, size * -0.6);
           EnemyManager::Instance()->Add(m_eeEnemyType, size * -0.85, size * -0.6);
           EnemyManager::Instance()->Add(PriestEnemy, 0, 0);
       }
   }

   return true;
}

bool EnemyGroup::SpawnGroup(Ogre::FrameEvent evt)
{
    float size = Tile::GetDefaultSize() / 2;

    if(m_bHasPriest)
    {
        if(!m_bFirstHalfSent)
        {
            EnemyManager::Instance()->Add(m_eeEnemyType, size * 0.65, size * 0.4);
            EnemyManager::Instance()->Add(m_eeEnemyType, size * -0.65, size * 0.4);
            EnemyManager::Instance()->Add(PriestEnemy, 0, size * -0.3);

            m_bFirstHalfSent = true;
        }
        else if(m_fTimeWaited > (m_fSendTime + 1))
        {
            EnemyManager::Instance()->Add(m_eeEnemyType, size * 0.65, size * 0.4);
            EnemyManager::Instance()->Add(m_eeEnemyType, size * -0.65, size * 0.4);
            EnemyManager::Instance()->Add(m_eeEnemyType, 0, size * -0.3);

            return true;
        }
    }
    else
    {
        if(!m_bFirstHalfSent)
        {
            EnemyManager::Instance()->Add(m_eeEnemyType, size * 0.5, size * -0.4);
            EnemyManager::Instance()->Add(m_eeEnemyType, size * -0.5, size * -0.4);
            EnemyManager::Instance()->Add(m_eeEnemyType, 0, size * 0.6);

            m_bFirstHalfSent = true;
        }
        else if(m_fTimeWaited > (m_fSendTime + 1))
        {
            EnemyManager::Instance()->Add(m_eeEnemyType, size * 0.5, 0);
            EnemyManager::Instance()->Add(m_eeEnemyType, size * -0.5, 0);

            return true;
        }
    }

    return false;
}

