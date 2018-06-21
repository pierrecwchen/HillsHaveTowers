//-----------------------------------------------------------------------------
// TeslaTower.h                                             By: Keenan Johnston
// 
// Represents an electric tower.  Overridden from BaseTower.
//-----------------------------------------------------------------------------

#ifndef TESLATOWER_H
#define TESLATOWER_H

#include "BaseTower.h"

class TeslaTower : public BaseTower
{
    public:
        friend class TowerManager;

        static Currency GetCost(int p_iLevel) { return m_cBuildCost[p_iLevel]; }

        void Upgrade();
        void Salvage();
        void Attack(Ogre::Real p_rTimeSinceLastFrame);
        void Update(const Ogre::FrameEvent& p_feEvent);


    private:
        // Static variable to represent the initial cost of this tower
        static Currency m_cBuildCost[];

        Model* m_pModel3;

        // number of times the lightning will jump to a new target
        int m_iJumpCount;

        float m_fTimeSpentFiring;

        int m_iLoopChannel;
        bool m_bIsLoopingSound;

        Ogre::SceneNode* m_pParticleNode;

        TeslaTower(Tile* p_pTile, bool p_bIsNight);
};

#endif