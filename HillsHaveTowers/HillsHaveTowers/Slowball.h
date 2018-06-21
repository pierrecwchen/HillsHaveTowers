//-----------------------------------------------------------------------------
// Slowball.h                                               By: Keenan Johnston
// 
// Represents a ice ball projectile.  Derived from the base projectile class
// with an area of effect and a slowing factor instead of damage.
//
// All code by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef SLOWBALL_H
#define SLOWBALL_H

#include "Projectile.h"

class Slowball : public Projectile
{
    public:
        friend class ProjectileManager;

        virtual void Update(const Ogre::FrameEvent& p_feEvent);
        
    private:
        // Slow factor and particles used by this projectile
        float m_fSlow;
        Ogre::ParticleEmitter* m_pSnow;
        std::vector<Ogre::ParticleSystem*> m_vParticles;

        Slowball(Ogre::Vector3 p_v3Loc, float p_fSpeed, BaseEnemy* p_pEnemy, 
                 float p_fSlow, float p_fRadius, Ogre::Vector3 p_v3Offset);
};

#endif