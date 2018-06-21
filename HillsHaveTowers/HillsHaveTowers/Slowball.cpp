//-----------------------------------------------------------------------------
// Slowball.cpp                                             By: Keenan Johnston
// 
// Implementation file for Slowball.h
//-----------------------------------------------------------------------------

#include "Slowball.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Slowball::Slowball(Ogre::Vector3 p_v3Loc, float p_fSpeed, BaseEnemy* p_pEnemy, 
                   float p_fSlow, float p_fRadius, Ogre::Vector3 p_v3Offset)
{
    m_v3StartPos = p_v3Loc + p_v3Offset;
    m_fSpeed = p_fSpeed;
    m_v3EndPos = p_pEnemy->GetLocation();
    m_fSlow = p_fSlow;
    m_fRadius = p_fRadius;
    m_pEnemy = p_pEnemy;
    m_pSnow = 0;

    m_fGravity = 0;

    m_bCanDelete = false;
    m_bParticleActive = false;

    // Set up the visual representation of the projectile
    m_pModel = new Model("Iceball.mesh");
    //m_pModel->Scale(Ogre::Vector3(0.01f));
    m_pModel->Translate(m_v3StartPos);

    // Give this projectile a unique name so it can be referenced when needed
    m_sParticleName = "Explosion" + m_pModel->GetSceneNode()->getName();

    // Create the particle trail this projectile will use
    m_pParticleSystem = Ogre::Root::getSingletonPtr()->
                        getSceneManager(BaseApplication::GetSceneManagerName())->
                        createParticleSystem(m_pModel->GetSceneNode()->getName() + "SnowTrail", "SnowflakeTrail");
    m_pModel->GetSceneNode()->attachObject(m_pParticleSystem);

    // Temporary translation of bullet to point
    m_pModel->TranslateToWithGravity(m_fSpeed, m_v3EndPos);
}

//-----------------------------------------------------------------------------
// Update the projectile and particles as required
//-----------------------------------------------------------------------------
void Slowball::Update(const Ogre::FrameEvent& p_feEvent)
{
    if (m_bParticleActive) // check if the projectile's particle is active
    {
        m_fParticleTime -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

        // Test for the snow particles
        if (m_pSnow == NULL)
            m_pSnow = Ogre::Root::getSingletonPtr()->
                        getSceneManager(BaseApplication::GetSceneManagerName())->
                        getParticleSystem(m_pModel->GetSceneNode()->getName() + "Snowflakes")->getEmitter(0);

        if (m_pSnow->getParticleVelocity() > 0)
        {
            m_pSnow->setParticleVelocity(m_pSnow->getParticleVelocity() - p_feEvent.timeSinceLastFrame*10);
        }


        // When the particle timers are up, delete them
        if (m_fParticleTime <= 0)
        {
            Ogre::Root::getSingletonPtr()->
              getSceneManager(BaseApplication::GetSceneManagerName())->
              destroyParticleSystem(m_pModel->GetSceneNode()->getName() + "Snowflakes");
            Ogre::Root::getSingletonPtr()->
              getSceneManager(BaseApplication::GetSceneManagerName())->
              destroyParticleSystem(m_pModel->GetSceneNode()->getName() + "ImpactBlue");
            Ogre::Root::getSingletonPtr()->
              getSceneManager(BaseApplication::GetSceneManagerName())->
              destroyParticleSystem(m_pModel->GetSceneNode()->getName() + "ShockwaveBlue");
            Ogre::Root::getSingletonPtr()->
              getSceneManager(BaseApplication::GetSceneManagerName())->
              destroyParticleSystem(m_pModel->GetSceneNode()->getName() + "SnowBlast");
            m_vParticles.clear();
            Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->
              destroyParticleSystem(m_pParticleSystem);
            m_bCanDelete = true;
        }
    }
    else if (!m_pModel->IsTranslating()) // check if projectile has reached it's destination
    {
        // If it has, we want to get rid of the projectile and create a particle effect
        m_pModel->GetSceneNode()->setVisible(false);

        // Stop the trail effect
        m_pParticleSystem->setEmitting(false);

        // Deal damage to the enemy
        Hills::EventManager::AddEvent(Hills::GameEvent(Hills::GE_SlowAttack,
                                                                           m_pModel->GetLocation(), 
                                                                           m_fRadius, 
                                                                           m_fSlow));
        SoundManager::Play3DSound("freezeExplosion.wav", m_pModel->GetLocation(), 0.75f);

        // Create the particle effects for the projectile's explosion
        m_pModel->GetSceneNode()->attachObject(Ogre::Root::getSingletonPtr()->
                               getSceneManager(BaseApplication::GetSceneManagerName())->
                               createParticleSystem(m_pModel->GetSceneNode()->getName() + "Snowflakes", "Snowflakes"));
        m_pModel->GetSceneNode()->attachObject(Ogre::Root::getSingletonPtr()->
                               getSceneManager(BaseApplication::GetSceneManagerName())->
                               createParticleSystem(m_pModel->GetSceneNode()->getName() + "ImpactBlue", "ImpactBlue"));
        m_pModel->GetSceneNode()->attachObject(Ogre::Root::getSingletonPtr()->
                               getSceneManager(BaseApplication::GetSceneManagerName())->
                               createParticleSystem(m_pModel->GetSceneNode()->getName() + "ShockwaveBlue", "ShockwaveBlue"));
        m_pModel->GetSceneNode()->attachObject(Ogre::Root::getSingletonPtr()->
                               getSceneManager(BaseApplication::GetSceneManagerName())->
                               createParticleSystem(m_pModel->GetSceneNode()->getName() + "SnowBlast", "SnowBlast"));

        m_bParticleActive = true;
        m_fParticleTime = 1.5f;
    }
    
    m_pModel->Update(p_feEvent);
}
