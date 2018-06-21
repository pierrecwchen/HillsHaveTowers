//-----------------------------------------------------------------------------
// CannonTower.h                                            By: Keenan Johnston
// 
// Represents a signal fire that lights an area.  Overridden from BaseTower.
//-----------------------------------------------------------------------------

#ifndef TORCHTOWER_H
#define TORCHTOWER_H

#include "BaseTower.h"

class TorchTower : public BaseTower
{
    public:
        friend class TowerManager;

        static Currency GetCost(int p_iLevel) { return m_cBuildCost[p_iLevel]; }

        void Upgrade();
        void Salvage();
        void Update(const Ogre::FrameEvent& p_feEvent);

        virtual void HandleEvent(const Hills::GameEvent& p_geGameEvent);

    private:
        // Static variable to represent the initial cost of this tower
        static Currency m_cBuildCost[];

        Ogre::SceneNode* m_pParticleNode;
        Ogre::ParticleSystem* m_pParticleSystem1, *m_pParticleSystem2;

        TorchTower(Tile* p_pTile, bool p_bIsNight);
};

#endif