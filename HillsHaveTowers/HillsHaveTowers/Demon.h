//-----------------------------------------------------------------------------
// Demon.h                                                  By: Keenan Johnston
// 
// Represents the demon boss enemy within the game.  Derived from BaseEnemy
// with several changes for additional behaviour.
//
// All code written by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef DEMON_H
#define DEMON_H

#include "BaseEnemy.h"
#include "Tile.h"

class Demon : public BaseEnemy
{
    public:
        friend class EnemyManager;

        virtual void Update(const Ogre::FrameEvent& p_feEvent);
    
    private:
        // Private members
        float m_fAttackTime, m_fAnimationTime;
        bool m_bAttackPlaying, m_bAttackEventFired;

        Ogre::Camera* m_pCamera;
        Ogre::Vector3 m_vCameraPos;
        bool m_bCameraShaking;
        float m_fAmplitude, m_fFrequency, m_fShakeTime;

        // Material for phasing out with transparency
        Ogre::MaterialPtr m_pMaterial;

        // Particles for fire attack
        Ogre::ParticleSystem* m_pFireWave, *m_pFireSparks, *m_pFireExplosion, *m_pFireImpact, *m_pFireFlash;

        // Other particles
        Ogre::ParticleSystem* m_pFire, *m_pGroundFire;

        // Node for fire particle
        Ogre::SceneNode* m_pFireNode;

        int m_iFireChannel;

        // Methods to handle uniques deaths from towers
        virtual void DieByFire();
        virtual void PlayEscapeSound();

        virtual bool IsIdentified() { return true; }

        // Constructor
        Demon(std::list<Tile> p_lShortestPath, float p_fXOffset = 1000, float p_fZOffset = 1000);
};

#endif