//-----------------------------------------------------------------------------
// MagicTower.h                                             By: Keenan Johnston
// 
// Represents a magic tower.  Overridden from BaseTower.
//-----------------------------------------------------------------------------

#ifndef MAGICTOWER_H
#define MAGICTOWER_H

#include "BaseTower.h"

class MagicTower : public BaseTower
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

        Ogre::ParticleSystem* m_pParticleSystem1, *m_pParticleSystem2, *m_pParticleSystem3;

        MagicTower(Tile* p_pTile, bool p_bIsNight);
};

#endif