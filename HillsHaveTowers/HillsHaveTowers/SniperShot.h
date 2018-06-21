//-----------------------------------------------------------------------------
// SniperShot.h                                             By: Keenan Johnston
// 
// Represents a trail of smoke from a sniper rifle shot.  Overridden from the
// base projectile class, defining several extra variables to represent the
// smoke trail as it is created.
//
// All code by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef SNIPERSHOT_H
#define SNIPERSHOT_H

#include "Projectile.h"

class SniperShot : public Projectile
{
    public:
        friend class ProjectileManager;

        virtual void DestroyModel();
        virtual void Update(const Ogre::FrameEvent& p_feEvent);
        
    private:
        // The smoke model itself and its material
        Ogre::ManualObject* m_pTrailModel;
        Ogre::SceneNode* m_pSceneNode;
        Ogre::MaterialPtr m_pMaterial;
        Ogre::ColourValue m_cAmbient, m_cDiffuse, m_cSpecular, m_cEmissive;

        // Values related to the segments of the smoke
        bool m_bFirstSegment;
        float m_fSegments, m_fSegmentToDraw;
        float m_fTimeUntilNextSegment;

        // Vectors for orienting the smoke
        Ogre::Vector3 m_v3Source;
        Ogre::Vector3 direction;
        Ogre::Vector3 position;

        SniperShot(Ogre::Vector3 p_v3Loc, float p_fSpeed, BaseEnemy* p_pEnemy, float p_fDamage, Ogre::Vector3 p_v3Offset);

        // Helper method which generates the smoke trail
        void DrawTrail();
};

#endif