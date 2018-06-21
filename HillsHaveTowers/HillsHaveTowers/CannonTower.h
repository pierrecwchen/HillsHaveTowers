//-----------------------------------------------------------------------------
// CannonTower.h                                            By: Keenan Johnston
// 
// Represents a cannon tower.  Overridden from BaseTower.
//-----------------------------------------------------------------------------

#ifndef CANNONTOWER_H
#define CANNONTOWER_H

#include "BaseTower.h"

class CannonTower : public BaseTower
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

        // Splash radius
        float m_fRadius;

        CannonTower(Tile* p_pTile, bool p_bIsNight);
        CannonTower(float p_fRange, float p_fAtkRate, float p_fDamage, float p_fBuildTime,
                    std::string p_sMesh, Ogre::SceneManager* p_smManager);

        void PlaySound();
};

#endif