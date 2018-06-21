//-----------------------------------------------------------------------------
// ArrowShot.h                                              By: Keenan Johnston
// 
// Represents a arrow projectile.  Derived from the base projectile class with
// no major changes.
//
// All code by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef ARROWSHOT_H
#define ARROWSHOT_H

#include "Projectile.h"


class ArrowShot : public Projectile
{
    public:
        friend class ProjectileManager;

        virtual void Update(const Ogre::FrameEvent& p_feEvent);
        
    private:
        ArrowShot(Ogre::Vector3 p_v3Loc, float p_fSpeed, BaseEnemy* p_pEnemy, 
                  float p_fDamage, Ogre::Vector3 p_v3Offset);

        void PlayHitSound();
};

#endif