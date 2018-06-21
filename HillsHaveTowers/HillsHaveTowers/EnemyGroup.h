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

#ifndef ENEMYGROUP_H
#define ENEMYGROUP

#include "EnemyManager.h"

class EnemyGroup
{
public:
    EnemyGroup();

    //Assignment constructor
    EnemyGroup(Enemies p_eeEnemyType, int p_iCount, float p_fLaunchTime, bool p_bWithPriest);

    //Returns true if all enemies have been sent in that group
    inline bool IsFinished() { return m_bAllEnemiesSent; }

    void Reset();

    void Update(Ogre::FrameEvent evt);

private:

    bool SpawnGroup();
    bool SpawnGroup(Ogre::FrameEvent evt);

    //What kind of enemy, how many, and when to send them
    Enemies m_eeEnemyType;
    int m_iCount;
    float m_fSendTime, m_fTimeWaited;
    bool m_bHasPriest; 

    //True if all enemies are sent
    bool m_bAllEnemiesSent;
    bool m_bFirstHalfSent; 

	//bool m_bAnimStart;
};

#endif