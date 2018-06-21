//-----------------------------------------------------------------------------
// MagicBolt.cpp                                            By: Keenan Johnston
// 
// Implementation file for MagicBolt.h
//-----------------------------------------------------------------------------

#include "MagicBolt.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
MagicBolt::MagicBolt(Ogre::Vector3 p_v3Loc, float p_fSpeed, BaseEnemy* p_pEnemy, float p_fDamage, Ogre::Vector3 p_v3Offset)
{
    m_v3StartPos = p_v3Loc + p_v3Offset;
    m_fSpeed = p_fSpeed;
    m_v3EndPos = p_pEnemy->GetLocation();
    m_fDamage = p_fDamage;
    m_pEnemy = p_pEnemy;

    m_fGravity = 0;

    m_bCanDelete = false;
    m_bParticleActive = false;

    // Set up the visual representation of the projectile
    m_pModel = new Model("Iceball.mesh");
    m_pModel->Scale(Ogre::Vector3(0.01, 0.01, 0.01));
    m_pModel->Translate(m_v3StartPos);

    Ogre::Vector3 m_v3Source = m_pModel->GetSceneNode()->getOrientation() * Ogre::Vector3::UNIT_Z;
    Ogre::Vector3 direction = (m_v3EndPos - m_v3StartPos);
	direction.normalise();
    Ogre::Quaternion quat = m_v3Source.getRotationTo(direction);
    m_pModel->GetSceneNode()->rotate(quat);
    m_pModel->GetSceneNode()->yaw(Ogre::Degree(-90));
    

    // Give this projectile a unique name so it can be referenced when needed
    m_sParticleName = m_pModel->GetSceneNode()->getName() + "MagicBlast";

    m_pParticleSystem = Ogre::Root::getSingletonPtr()->
                        getSceneManager(BaseApplication::GetSceneManagerName())->
                        createParticleSystem(m_sParticleName, "MagicBlast");

    // Create the moving projectile particle
    m_pParticleSystem2 = Ogre::Root::getSingletonPtr()->
                        getSceneManager(BaseApplication::GetSceneManagerName())->
                        createParticleSystem(m_pModel->GetSceneNode()->getName() + "MagicTrail", "MagicTrail");
    m_pModel->GetSceneNode()->attachObject(m_pParticleSystem2);

    // Create a shockwave particle for impact
    m_pParticleSystem3 = Ogre::Root::getSingletonPtr()->
                        getSceneManager(BaseApplication::GetSceneManagerName())->
                        createParticleSystem(m_pModel->GetSceneNode()->getName() + "MagicWave", "ShockwavePurple");

    // Translate the projectile to the enemy's location
    m_pModel->TranslateToWithSpeed(m_fSpeed, m_v3EndPos);
}

//-----------------------------------------------------------------------------
// Update the projectile and particles as required
//-----------------------------------------------------------------------------
void MagicBolt::Update(const Ogre::FrameEvent& p_feEvent)
{
    if (m_bParticleActive) // check if the projectile's particle is active
    {
        m_fParticleTime -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

        // Destroy the trail if all of its particles are finished
        if (m_pParticleSystem2->getNumParticles() == 0) 
            Ogre::Root::getSingletonPtr()->
              getSceneManager(BaseApplication::GetSceneManagerName())->
              destroyParticleSystem(m_pModel->GetSceneNode()->getName() + "MagicTrail");

        // Destroy the particles and flag this object for deletion
        if (m_fParticleTime <= 0)
        {            
            Ogre::Root::getSingletonPtr()->
              getSceneManager(BaseApplication::GetSceneManagerName())->
              destroyParticleSystem(m_sParticleName);
            Ogre::Root::getSingletonPtr()->
              getSceneManager(BaseApplication::GetSceneManagerName())->
              destroyParticleSystem(m_pModel->GetSceneNode()->getName() + "MagicWave");
            m_bCanDelete = true;
        }
        // Stop the impact particles from emitting
        else if (m_fParticleTime <= 1.35)
        {
            m_pParticleSystem->setEmitting(false);
            m_pParticleSystem3->setEmitting(false);
        }
    }
    else if (!m_pModel->IsTranslating()) // check if projectile has reached it's destination
    {
        // Stop the trail from emitting particles
        m_pParticleSystem2->setEmitting(false);

        // Deal damage to the enemy
        m_pEnemy->Attacked(m_fDamage, false, AT_STANDARD);
        SoundManager::Play3DSound("magicHit.wav", m_pModel->GetLocation(), 0.5f);

        m_pParticleSystem->setBoundsAutoUpdated(0);  // Particle effeciency needs improvements
        m_bParticleActive = true;
        m_fParticleTime = 1.5f;
        m_pModel->GetSceneNode()->attachObject(m_pParticleSystem);
        m_pModel->GetSceneNode()->attachObject(m_pParticleSystem3);
    }
    else
    {
        // Get current position of enemy to home in
        if (m_pEnemy->GetState() != DeadState)
        {
            m_pModel->StopMovement();
            m_pModel->TranslateToWithSpeed(m_fSpeed, m_pEnemy->GetLocation());

            Ogre::Vector3 m_v3Source = m_pModel->GetSceneNode()->getOrientation() * Ogre::Vector3::UNIT_Z;
            Ogre::Vector3 direction = (m_pEnemy->GetLocation() - m_pModel->GetLocation());
	        direction.normalise();
            Ogre::Quaternion quat = m_v3Source.getRotationTo(direction);
            m_pModel->GetSceneNode()->rotate(quat);
            m_pModel->GetSceneNode()->yaw(Ogre::Degree(-90));
        }
    }
    
    m_pModel->Update(p_feEvent);
}