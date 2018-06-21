//-----------------------------------------------------------------------------
// Projectile.h                                             By: Keenan Johnston
// 
// Represents a projectile within the game.  Is created at one location,
// translates to another, deals damage to one or more enemies upon reaching it
// and plays an effect.  Works in conjunction with ProjectileManager and cannot
// be instantiated on its own.
//
// All code by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Model.h"
#include "BaseEnemy.h"
#include "EventManager.h"
#include "SoundManager.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
class Projectile
{
    public:
        friend class ProjectileManager;

        // Flag checks
        inline bool CanDelete() { return m_bCanDelete; }
        inline bool HasHit() { return m_bParticleActive; }

        // Clean up all OGRE-related variables here
        virtual void DestroyModel();
        
        // Update the current state of this projectile
		virtual void Update(const Ogre::FrameEvent& p_feEvent);

    protected:
        // Base attributes
        Ogre::Vector3 m_v3StartPos, m_v3CurrentPos, m_v3EndPos;
        float m_fSpeed, m_fDamage, m_fRadius;
        Model* m_pModel;
        float m_fGravity;

        // The enemy that is being targetted
        BaseEnemy* m_pEnemy;

        // Particle effects
        Ogre::ParticleSystem* m_pParticleSystem;
        Ogre::ParticleSystem* m_pParticleSystem2;
        std::string m_sParticleName, m_sParticleName2;
        float m_fParticleTime;
        bool m_bParticleActive;

        // Ribbon effect
        Ogre::RibbonTrail* m_pTrail;

        // Deletion flag
        bool m_bCanDelete;

        // Private constructor, only accessible by ProjectileManager
        Projectile();
        Projectile(Ogre::Vector3 p_v3Loc, float p_fSpeed, BaseEnemy* p_pEnemy, float p_fDamage);
};

#endif