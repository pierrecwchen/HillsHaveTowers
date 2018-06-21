//-----------------------------------------------------------------------------
// SniperTower.h                                            By: Keenan Johnston
// 
// Represents a sniper tower.  Overridden from BaseTower.
//-----------------------------------------------------------------------------

#ifndef SNIPERTOWER_H
#define SNIPERTOWER_H

#include "BaseTower.h"

class SniperTower : public BaseTower
{
    public:
        friend class TowerManager;

        static Currency GetCost(int p_iLevel) { return m_cBuildCost[p_iLevel]; }

        void Upgrade();
        void Salvage();
        void Attack(Ogre::Real p_rTimeSinceLastFrame);
        void Update(const Ogre::FrameEvent& p_feEvent);

        virtual void HandleEvent(const Hills::GameEvent& p_geGameEvent);

    private:
        // Static variable to represent the initial cost of this tower
        static Currency m_cBuildCost[];

        SniperTower(Tile* p_pTile, bool p_bIsNight);
        SniperTower(float p_fRange, float p_fAtkRate, float p_fDamage, float p_fBuildTime,
                    std::string p_sMesh, Ogre::SceneManager* p_smManager);
};

#endif