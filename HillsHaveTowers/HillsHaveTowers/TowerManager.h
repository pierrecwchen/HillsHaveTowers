//-----------------------------------------------------------------------------
// TowerManager.h                                           By: Keenan Johnston
// 
// Manager class for towers.  Handles creating, updating and deleting towers
// as required.  Also has a few utility methods for setting and getting the
// tower selected by the user used for priority target setting.
//
// All code by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef TOWERMANAGER_H
#define TOWERMANAGER_H

#include <vector>
#include <iostream>

#include "ArrowTower.h"
#include "CannonTower.h"
#include "SniperTower.h"
#include "BurstTower.h"
#include "SlowTower.h"
#include "TeslaTower.h"
#include "MagicTower.h"
#include "TorchTower.h"
#include "SelectableManager.h"

enum Towers {eArrowTower, eCannonTower, eSniperTower, eBurstTower, eSlowTower, T_TESLA, T_MAGIC, T_TORCH};

class TowerManager : public Hills::Recipient
{
    public:
        friend class BaseTower;
        
        static TowerManager* Instance();

        // Add and delete methods
        BaseTower* Add(Towers p_eType,  Tile* p_pTile);
        BaseTower* Add(Towers p_eType, float p_fRange, float p_fAtkRate, float p_fDamage,
                       float p_fBuildTime, std::string p_sMesh,
                       Ogre::SceneManager* p_smManager);
        void Delete(BaseTower* p_pTower);
		void DeleteAll();

        // Update all towers
        void Update(const Ogre::FrameEvent& p_feEvent);
            
        // Getter methods
		//static BaseTower* Get(/*something*/);
		inline std::vector<BaseTower*> GetAll() { return m_vTowers; }
		inline int Size() { return m_vTowers.size(); }

        // Utility methods
        inline void SetSelectedTower(BaseTower* p_pTower) { m_pSelectedTower = p_pTower; }
        inline BaseTower* GetSelectedTower() { return m_pSelectedTower; }

        void HandleEvent(const Hills::GameEvent& p_geGameEvent);
         
    private:
        static TowerManager* m_tmInstance;

        TowerManager();
        ~TowerManager() {}

        // All towers on the field and the tower currently selected by the user
        std::vector<BaseTower*> m_vTowers;
        BaseTower* m_pSelectedTower;

        bool m_bIsNight;
};

#endif