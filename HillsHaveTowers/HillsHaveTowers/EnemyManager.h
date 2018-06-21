//-----------------------------------------------------------------------------
// EnemyManager.h                                           By: Keenan Johnston
// 
// Manager class for enemies.  Handles creating, updating and deleting enemies
// as required.  Also has a few utility methods for checking and getting
// enemies within a given circular area.
//
// All code by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef ENEMYMANAGER_H
#define ENEMYMANAGER_H

#include <cmath>
#include <vector>
#include <iostream>

#include "Soldier.h"
#include "Scout.h"
#include "Priest.h"
#include "Knight.h"
#include "Demon.h"

#include "TileSet.h"
#include "SelectableManager.h"
#include "EventManager.h"

enum Enemies {SoldierEnemy, ScoutEnemy, PriestEnemy, KnightEnemy, ET_DEMON};

class EnemyManager
{
	public:
        friend class BaseEnemy;
        
        static EnemyManager* Instance();

        // Add and delete methods
        BaseEnemy* Add(Enemies p_eType);
        BaseEnemy* Add(Enemies p_eType, float p_fXOffset, float p_fZOffset); 
        void Delete(BaseEnemy* p_eEnemy);
		void DeleteAll();

        // Update all enemies
        void Update(const Ogre::FrameEvent& p_feEvent);

        // Kill enemy at the front of the vector (test)
        inline void Kill() { m_vEnemies.at(0)->SetHP(0); }
            
        // Getter methods
		//static BaseEnemy* Get(/*something*/);
		inline std::vector<BaseEnemy*> GetAll() { return m_vEnemies; }
		inline int Size() { return m_vEnemies.size(); }
		
		// Range checking methods
		bool EnemiesInRange(Ogre::Vector3 p_v3Loc, float p_fRange, float p_fBlindness = 1.0f);
		std::vector<BaseEnemy*> GetEnemiesInRange(Ogre::Vector3 p_v3Loc, float p_fRange, float p_fBlindness = 1.0f);

	private:

        static EnemyManager* m_emInstance;

        EnemyManager() {}
        ~EnemyManager() {}
        // All enemies currently on the field
		std::vector<BaseEnemy*> m_vEnemies;
};

#endif