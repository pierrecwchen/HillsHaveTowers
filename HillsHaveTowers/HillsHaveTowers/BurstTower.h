//-----------------------------------------------------------------------------
// BurstTower.h                                             By: Keenan Johnston
// 
// Represents a fireburst tower.  Overridden from BaseTower.
//-----------------------------------------------------------------------------

#ifndef BURSTTOWER_H
#define BURSTTOWER_H

#include "BaseTower.h"

class BurstTower : public BaseTower
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

        float m_fTimeSpentFiring;

        int m_iLoopChannel;
        bool m_bIsLoopingSound;

        //Model* m_pModel2;
        Ogre::SceneNode* m_pParticleNode;;

        BurstTower(Tile* p_pTile, bool p_bIsNight);
        BurstTower(float p_fRange, float p_fAtkRate, float p_fDamage, float p_fBuildTime,
                   std::string p_sMesh, Ogre::SceneManager* p_smManager);
};

#endif