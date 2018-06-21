//-----------------------------------------------------------------------------
// Cannonball.cpp                                           By: Keenan Johnston
// 
// Implementation file for Cannonball.h
//-----------------------------------------------------------------------------

#include "Cannonball.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Cannonball::Cannonball(Ogre::Vector3 p_v3Loc, float p_fSpeed, BaseEnemy* p_pEnemy, 
                       float p_fDamage, float p_fRadius, Ogre::Vector3 p_v3Offset)
{
    m_v3StartPos = p_v3Loc + p_v3Offset;
    m_fSpeed = p_fSpeed;
    m_v3EndPos = p_pEnemy->GetLocation();
    m_fDamage = p_fDamage;
    m_fRadius = p_fRadius;
    m_pEnemy = p_pEnemy;

    m_fGravity = 0;

    m_bCanDelete = false;
    m_bParticleActive = false;

    // Set up the visual representation of the projectile
    m_pModel = new Model("Cannonball.mesh");
    //m_pModel->Scale(Ogre::Vector3(0.1f));
    m_pModel->Translate(m_v3StartPos);

    // Give this projectile a unique name so it can be referenced when needed
    m_sParticleName = "Explosion" + m_pModel->GetSceneNode()->getName();

    // Temporary translation of bullet to point
    m_pModel->TranslateToWithGravity(m_fSpeed, m_v3EndPos);
}

//-----------------------------------------------------------------------------
// Update the projectile and particles as required
//-----------------------------------------------------------------------------
void Cannonball::Update(const Ogre::FrameEvent& p_feEvent)
{
    if (m_bParticleActive) // check if the projectile's particle is active
    {
        m_fParticleTime -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

        // When the particle timers are up, delete all of them
        if (m_fParticleTime <= 0)
        {
            Ogre::Root::getSingletonPtr()->
              getSceneManager(BaseApplication::GetSceneManagerName())->
              destroyParticleSystem(m_pModel->GetSceneNode()->getName() + "Flash");
            Ogre::Root::getSingletonPtr()->
              getSceneManager(BaseApplication::GetSceneManagerName())->
              destroyParticleSystem(m_pModel->GetSceneNode()->getName() + "Sparktrail");
            Ogre::Root::getSingletonPtr()->
              getSceneManager(BaseApplication::GetSceneManagerName())->
              destroyParticleSystem(m_pModel->GetSceneNode()->getName() + "Shockwave");
            Ogre::Root::getSingletonPtr()->
              getSceneManager(BaseApplication::GetSceneManagerName())->
              destroyParticleSystem(m_pModel->GetSceneNode()->getName() + "Smoketrail");
            Ogre::Root::getSingletonPtr()->
              getSceneManager(BaseApplication::GetSceneManagerName())->
              destroyParticleSystem(m_pModel->GetSceneNode()->getName() + "Explosion");
            m_bCanDelete = true;
            m_vParticles.clear();
        }
    }
    else if (!m_pModel->IsTranslating()) // check if projectile has reached it's destination
    {
        // If it has, we want to get rid of the projectile and create a particle effect
        m_pModel->GetSceneNode()->setVisible(false);

        // Deal damage to the enemy
        Hills::EventManager::AddEvent(Hills::GameEvent(Hills::GE_AreaAttack,
                                                                           Hills::AOE_Sphere,
                                                                           m_pModel->GetLocation(),
                                                                           m_fRadius,
                                                                           m_fDamage,
                                                                           0));
        PlayHitSound();

        // Create the particle effects for this projectile
        m_pModel->GetSceneNode()->attachObject(Ogre::Root::getSingletonPtr()->
                               getSceneManager(BaseApplication::GetSceneManagerName())->
                               createParticleSystem(m_pModel->GetSceneNode()->getName() + "Flash", "Flash"));
        m_pModel->GetSceneNode()->attachObject(Ogre::Root::getSingletonPtr()->
                               getSceneManager(BaseApplication::GetSceneManagerName())->
                               createParticleSystem(m_pModel->GetSceneNode()->getName() + "Sparktrail", "Sparktrail"));
        m_pModel->GetSceneNode()->attachObject(Ogre::Root::getSingletonPtr()->
                               getSceneManager(BaseApplication::GetSceneManagerName())->
                               createParticleSystem(m_pModel->GetSceneNode()->getName() + "Shockwave", "Shockwave"));
        m_pModel->GetSceneNode()->attachObject(Ogre::Root::getSingletonPtr()->
                               getSceneManager(BaseApplication::GetSceneManagerName())->
                               createParticleSystem(m_pModel->GetSceneNode()->getName() + "Smoketrail", "Smoketrail"));
        m_pModel->GetSceneNode()->attachObject(Ogre::Root::getSingletonPtr()->
                               getSceneManager(BaseApplication::GetSceneManagerName())->
                               createParticleSystem(m_pModel->GetSceneNode()->getName() + "Explosion", "Explosion"));

        m_bParticleActive = true;
        m_fParticleTime = 0.5f;
    }
    // Spin the ball a bit each frame
    m_pModel->GetSceneNode()->pitch(Ogre::Degree(p_feEvent.timeSinceLastFrame*10));
    m_pModel->Update(p_feEvent);
}

void Cannonball::PlayHitSound()
{
    int selection = rand() % 3;

    std::string fileNames[3];

    fileNames[0] = "explode1.wav";
    fileNames[1] = "explode2.wav";
    fileNames[2] = "explode3.wav";

    SoundManager::Play3DSound(fileNames[selection], m_pModel->GetLocation(), 0.25f);
}