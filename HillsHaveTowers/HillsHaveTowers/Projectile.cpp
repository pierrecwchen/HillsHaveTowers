#include "Projectile.h"

//-----------------------------------------------------------------------------
// Default Constructor
//-----------------------------------------------------------------------------
Projectile::Projectile()
{

}
//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Projectile::Projectile(Ogre::Vector3 p_v3Loc, float p_fSpeed, BaseEnemy* p_pEnemy, float p_fDamage)
{
    m_v3StartPos = p_v3Loc;
    m_fSpeed = p_fSpeed;
    m_v3EndPos = p_pEnemy->GetLocation();
    m_fDamage = p_fDamage;
    m_pEnemy = p_pEnemy;

    m_fGravity = 0;

    m_bCanDelete = false;
    m_bParticleActive = false;

    // Set up the visual representation of the projectile
    m_pModel = new Model("Cannonball.mesh");
    m_pModel->Scale(Ogre::Vector3(0.1f));
    m_pModel->Translate(m_v3StartPos);

    // Give this projectile a unique name so it can be referenced when needed
    m_sParticleName = "Explosion" + m_pModel->GetSceneNode()->getName();

    // Ribbon trail test
    //m_pTrail = (Ogre::RibbonTrail*)Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->createMovableObject(m_pModel->GetSceneNode()->getName() + "Trail");
    m_pTrail = Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->createRibbonTrail(m_pModel->GetSceneNode()->getName() + "Trail");
    m_pTrail->setMaterialName("M_Transparency"/*"Examples/LightRibbonTrail"*/);
    m_pTrail->setTrailLength(3000);
    m_pTrail->setMaxChainElements(5);
    Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->getRootSceneNode()->attachObject(m_pTrail);

    m_pTrail->setInitialColour(0, 1.0, 1.0, 1.0, 0.5);
    m_pTrail->setColourChange(0, 1.0, 1.0, 1.0, 0.0);
    m_pTrail->setWidthChange(0, 10);
    m_pTrail->setInitialWidth(0, 5);
    m_pTrail->addNode(m_pModel->GetSceneNode());
    
    // Particle test
    //m_sParticleName2 = "Trail" + m_pModel->GetSceneNode()->getName();
    //m_pParticleSystem2 = Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->createParticleSystem(m_sParticleName2, "Trail");
    //m_pParticleSystem = Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->createParticleSystem(m_sParticleName, "CannonExplosion1");
    //m_pParticleSystem2->setBoundsAutoUpdated(0);  // Particle effeciency needs improvements
    //m_pModel->GetSceneNode()->attachObject(m_pParticleSystem2);

    // Temporary translation of bullet to point
    //m_pModel->TranslateToWithSpeed(m_fSpeed, m_v3EndPos);
    m_pModel->TranslateTo(m_v3EndPos);
}
//-----------------------------------------------------------------------------
// Use OGRE to delete model-related data before destroying this object
//-----------------------------------------------------------------------------
void Projectile::DestroyModel()
{
	m_pEnemy = 0;
	m_pParticleSystem = 0;
	m_pParticleSystem2 = 0;
	m_pTrail = 0;

    m_pModel->CleanUp();
	delete m_pModel;
}

//-----------------------------------------------------------------------------
// Update the projectile and particles as required
//-----------------------------------------------------------------------------
void Projectile::Update(const Ogre::FrameEvent& p_feEvent)
{
    if (m_bParticleActive) // check if the projectile's particle is active
    {
        m_pTrail->_timeUpdate(p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed());
        m_pTrail->nodeUpdated(m_pModel->GetSceneNode());
        m_fParticleTime -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

        if (m_fParticleTime <= 0)
        {
            Ogre::Root::getSingletonPtr()->
              getSceneManager(BaseApplication::GetSceneManagerName())->
              destroyParticleSystem(m_sParticleName);
            m_bCanDelete = true;
        }
    }
    else if (!m_pModel->IsTranslating()) // check if projectile has reached it's destination
    {                   
        // If it has, we want to get rid of the projectile and create a particle effect
        m_pModel->GetSceneNode()->setVisible(false);

        // Deal damage to the enemy
        m_pEnemy->Attacked(m_fDamage, false, AT_STANDARD);

        m_pParticleSystem = Ogre::Root::getSingletonPtr()->
                            getSceneManager(BaseApplication::GetSceneManagerName())->
                            createParticleSystem(m_sParticleName, "CannonExplosion1");
        m_pParticleSystem->setBoundsAutoUpdated(0);  // Particle effeciency needs improvements
        m_bParticleActive = true;
        m_fParticleTime = 3.5f;
        m_pModel->GetSceneNode()->attachObject(m_pParticleSystem);
    }
    else
    {
        // If the enemy is dead, don't try to update their location again
        /*
        if (m_pEnemy->GetState() != DeadState)
        {
            m_pModel->StopMovement();
            m_pModel->TranslateToWithSpeed(m_fSpeed, m_pEnemy->GetLocation());
        }
        */
    }

    m_pModel->Update(p_feEvent);
    //m_pModel->GetSceneNode()->setPosition( m_pModel->GetSceneNode()->getPosition());
}