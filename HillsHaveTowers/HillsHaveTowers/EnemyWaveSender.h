//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
//EnemyWaveSender
//
//PURPOSE:
//This stores all the waves for an entire level
//
//CREDIT:
//Nick
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*

#ifndef ENEMYWAVESENDER_H
#define ENEMYWAVESENDER_H

#include <vector>
#include "EnemyWave.h"

class EnemyWaveSender
{
public:
    //The following methods are all extremely similar to EnemyWave's structure
    EnemyWaveSender();
    EnemyWaveSender(std::vector<EnemyWave*> p_vWaves);
	~EnemyWaveSender();

    inline void Add(EnemyWave* p_ewWave) { m_vWaves.push_back(p_ewWave); }

    inline bool AllEnemiesAreSent() { return m_bAllEnemiesSent; }

    void ResetTo(int p_iStartingWave);

    void Update(Ogre::FrameEvent evt);

private:
    std::vector<EnemyWave*> m_vWaves;
    int m_iWaveIndex;

    bool m_bAllEnemiesSent;
	bool m_bAnimStart;
};

#endif