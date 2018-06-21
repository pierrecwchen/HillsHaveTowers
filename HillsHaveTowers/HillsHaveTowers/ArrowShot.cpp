//-----------------------------------------------------------------------------
// ArrowShot.cpp                                            By: Keenan Johnston
// 
// Implementation file for ArrowShot.h
//-----------------------------------------------------------------------------

#include "ArrowShot.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
ArrowShot::ArrowShot(Ogre::Vector3 p_v3Loc, float p_fSpeed, BaseEnemy* p_pEnemy, float p_fDamage, Ogre::Vector3 p_v3Offset)
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
    m_pModel = new Model("Arrow.mesh");
    m_pModel->Scale(Ogre::Vector3(0.01, 0.01, 0.01));
    m_pModel->Translate(m_v3StartPos);

    Ogre::Vector3 m_v3Source = m_pModel->GetSceneNode()->getOrientation() * Ogre::Vector3::UNIT_Z;
    Ogre::Vector3 direction = (m_v3EndPos - m_v3StartPos);
	direction.normalise();
    Ogre::Quaternion quat = m_v3Source.getRotationTo(direction);
    m_pModel->GetSceneNode()->rotate(quat);
    m_pModel->GetSceneNode()->yaw(Ogre::Degree(-90));

    // Give this projectile a unique name so it can be referenced when needed
    m_sParticleName = "ArrowImpact" + m_pModel->GetSceneNode()->getName();

    // Translate the projectile to the enemy's location
    m_pModel->TranslateToWithSpeed(m_fSpeed, m_v3EndPos);
}

//-----------------------------------------------------------------------------
// Update the projectile and particles as required
//-----------------------------------------------------------------------------
void ArrowShot::Update(const Ogre::FrameEvent& p_feEvent)
{
    if (m_bParticleActive) // check if the projectile's particle is active
    {
        m_fParticleTime -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

         if (m_fParticleTime <= 0)
        {            
            Ogre::Root::getSingletonPtr()->
              getSceneManager(BaseApplication::GetSceneManagerName())->
              destroyParticleSystem(m_sParticleName);
            m_bCanDelete = true;
        }
        else if (m_fParticleTime <= 0.45)
        {
            m_pParticleSystem->setEmitting(false);
        }
    }
    else if (!m_pModel->IsTranslating()) // check if projectile has reached it's destination
    {
        // If it has, we want to get rid of the projectile and create a particle effect
        m_pModel->GetSceneNode()->setVisible(false);

        // Deal damage to the enemy
        m_pEnemy->Attacked(m_fDamage, false, AT_STANDARD);
        PlayHitSound();

        m_pParticleSystem = Ogre::Root::getSingletonPtr()->
                            getSceneManager(BaseApplication::GetSceneManagerName())->
                            createParticleSystem(m_sParticleName, "Impact");
        m_pParticleSystem->setBoundsAutoUpdated(0);  // Particle effeciency needs improvements
        m_bParticleActive = true;
        m_fParticleTime = 0.5f;
        m_pModel->GetSceneNode()->attachObject(m_pParticleSystem);
    }
    
    m_pModel->Update(p_feEvent);
}

void ArrowShot::PlayHitSound()
{
    if(m_pEnemy->HasArmor())
    {
        int selection = rand() % 3;

        std::string fileNames[3];

        fileNames[0] = "arrowHitArmor1.wav";
        fileNames[1] = "arrowHitArmor2.wav";
        fileNames[2] = "arrowHitArmor3.wav";

        SoundManager::Play3DSound(fileNames[selection], m_pModel->GetLocation(), 0.25f);
    }
    else
    {
        int selection = rand() % 4;

        std::string fileNames[4];

        fileNames[0] = "arrowHit1.wav";
        fileNames[1] = "arrowHit2.wav";
        fileNames[2] = "arrowHit3.wav";
        fileNames[3] = "arrowHit4.wav";

        SoundManager::Play3DSound(fileNames[selection], m_pModel->GetLocation(), 0.25f);
    }
}