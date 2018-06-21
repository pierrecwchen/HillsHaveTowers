//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
//EnemyWave
//
//PURPOSE:
//This class stores groups of enemies to signify one full wave in a level
//
//CREDIT:
//Nick
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*

#ifndef ENEMYWAVE_H
#define ENEMYWAVE_H

#include <vector>

#include "EnemyGroup.h"

class EnemyWave
{
public:
    EnemyWave(bool p_bNight = false);
    //Assignment constructor accepts a queue of groups
    EnemyWave(std::vector<EnemyGroup*> p_vGroups);
	~EnemyWave();

    inline void Add(EnemyGroup* p_egGroup) { m_vGroups.push_back(p_egGroup); }

    inline bool IsFinished() { return m_bWaveIsFinished; }
    inline bool IsNight() { return m_bNightWave; }
    

    void Reset();

    void Update(Ogre::FrameEvent evt);

	//By Pierre
	inline int GetGroupNum()const { return m_vGroups.size(); }

private:
    std::vector<EnemyGroup*> m_vGroups;
    int m_iGroupIndex;

    bool m_bWaveIsFinished;

    bool m_bNightWave;
};

#endif