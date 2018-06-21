//-----------------------------------------------------------------------------
// Projectile.h                                             By: Keenan Johnston
// 
// Represents a cannonball projectile.  Derived from the base projectile class,
// specifying an area of effect attack instead of a single target one.
//
// All code by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef CANNONBALL_H
#define CANNONBALL_H

#include "Projectile.h"

class Cannonball : public Projectile
{
    public:
        friend class ProjectileManager;

        // Overridden update method
        virtual void Update(const Ogre::FrameEvent& p_feEvent);
        
    private:
        // Particle systems used
        std::vector<Ogre::ParticleSystem*> m_vParticles;

        // Private constructor
        Cannonball(Ogre::Vector3 p_v3Loc, float p_fSpeed, BaseEnemy* p_pEnemy, 
                   float p_fDamage, float p_fRadius, Ogre::Vector3 p_v3Offset);
        
        // Helper method to play sound upon impact
        void PlayHitSound();
};

#endif