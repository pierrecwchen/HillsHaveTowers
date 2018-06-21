//-----------------------------------------------------------------------------
// ArrowTower.h                                             By: Keenan Johnston
// 
// Represents an arrow tower.  Overridden from BaseTower.
//-----------------------------------------------------------------------------

#ifndef ARROWTOWER_H
#define ARROWTOWER_H

#include "BaseTower.h"

class ArrowTower : public BaseTower
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

        ArrowTower(Tile* p_pTile, bool p_bIsNight);
        ArrowTower(float p_fRange, float p_fAtkRate, float p_fDamage, float p_fBuildTime,
                   std::string p_sMesh, Ogre::SceneManager* p_smManager);
};

#endif