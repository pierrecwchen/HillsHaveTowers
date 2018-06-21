//-----------------------------------------------------------------------------
// MagicBolt.h                                              By: Keenan Johnston
// 
// Represents a magic projectile.  Derived from the base projectile class with
// no major changes.
//
// All code by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef MAGICBOLT_H
#define MAGICBOLT_H

#include "Projectile.h"

class MagicBolt : public Projectile
{
    public:
        friend class ProjectileManager;

        virtual void Update(const Ogre::FrameEvent& p_feEvent);
        
    private:
        Ogre::ParticleSystem* m_pParticleSystem2, *m_pParticleSystem3;

        MagicBolt(Ogre::Vector3 p_v3Loc, float p_fSpeed, BaseEnemy* p_pEnemy, 
                  float p_fDamage, Ogre::Vector3 p_v3Offset);
};

#endif