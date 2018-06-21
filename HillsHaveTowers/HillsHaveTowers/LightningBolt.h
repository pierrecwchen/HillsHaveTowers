//-----------------------------------------------------------------------------
// LightningBolt.h                                          By: Keenan Johnston
// 
// Represents a bolt of lightning arcing from one location to another.
//
// All code by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef LIGHTNINGBOLT_H
#define LIGHTNINGBOLT_H

#include "Projectile.h"
#include "EnemyManager.h"
#include "Tile.h"

class LightningBolt : public Projectile
{
    public:
        friend class ProjectileManager;

        virtual void DestroyModel();
        virtual void Update(const Ogre::FrameEvent& p_feEvent);

        // Test constructor
        LightningBolt(Ogre::Vector3 p_v3Loc, Ogre::Vector3 p_v3EndLoc);
        
    private:
        // The bolt models
        std::vector<Ogre::ManualObject*> m_vBoltModels;
        std::vector<Ogre::SceneNode*> m_vBoltNodes;
        int m_iBoltCount;

        Ogre::SceneNode* m_pSceneNode;


        // Segments of the chain to draw
        float m_fSegments;

        // Vectors for orienting the lightning
        Ogre::Vector3 m_v3Source;
        Ogre::Vector3 direction;
        Ogre::Vector3 position;

        // Flag if enemy has been 
        bool m_bAttacked;

        // The number of jumps this bolt can make and the targets it has hit already
        int m_iJumpCount;
        std::vector<BaseEnemy*> m_vEnemiesHit;

        LightningBolt(Ogre::Vector3 p_v3Loc, int p_iJump, BaseEnemy* p_pEnemy, float p_fDamage, 
                      Ogre::Vector3 p_v3Offset);

        // Helper method which generates the lightning
        void DrawTrail();
};

#endif