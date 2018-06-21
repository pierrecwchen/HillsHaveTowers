//-----------------------------------------------------------------------------
// ProjectileManager.h                                      By: Keenan Johnston
// 
// Manager class for projectiles.  Handles creating, updating and deleting
// projectiles as required.
//
// All code by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef PROJECTILEMANAGER_H
#define PROJECTILEMANAGER_H

#include <cmath>
#include <vector>
#include <iostream>

#include "ArrowShot.h"
#include "Cannonball.h"
#include "SniperShot.h"
#include "Slowball.h"
#include "LightningBolt.h"
#include "MagicBolt.h"

enum ProjectileTypes {PT_ArrowShot, PT_CannonShot, PT_SlowShot, PT_SniperShot, PT_LIGHTNING, PT_MAGIC};

class ProjectileManager
{
    public:
        friend class Projectile;

        // Add and delete methods
        static Projectile* Add(ProjectileTypes p_eType, Ogre::Vector3 p_v3Loc, float p_fSpeed, 
                               BaseEnemy* p_pEnemy, float p_fDamage, Ogre::Vector3 p_v3Offset,
                               float p_fRadius = 0, float p_fSlow = 0, int p_iJump = 0);
        static void Delete(Projectile* p_pProjectile);
		static void DeleteAll();

        // Update all projectiles
        static void Update(const Ogre::FrameEvent& p_feEvent);

        // Getter methods
		//static Projectile* Get(/*something*/);
		static inline std::vector<Projectile*> GetAll() { return m_vProjectiles; }
		static inline int Size() { return m_vProjectiles.size(); }

    private:
        // All projectiles currently on the field
        static std::vector<Projectile*> m_vProjectiles;
};

#endif