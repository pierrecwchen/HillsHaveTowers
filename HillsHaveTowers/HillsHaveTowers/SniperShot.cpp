//-----------------------------------------------------------------------------
// SniperShot.cpp                                           By: Keenan Johnston
// 
// Implementation file for SniperShot.h
//-----------------------------------------------------------------------------

#include "SniperShot.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
SniperShot::SniperShot(Ogre::Vector3 p_v3Loc, float p_fSpeed, BaseEnemy* p_pEnemy, float p_fDamage, Ogre::Vector3 p_v3Offset)
{
    m_v3StartPos = p_v3Loc + p_v3Offset;
    m_fSpeed = p_fSpeed;
    m_v3EndPos = p_pEnemy->GetLocation();
    m_fDamage = p_fDamage;
    m_pEnemy = p_pEnemy;

    m_bCanDelete = false;
    m_bParticleActive = false;
    m_fParticleTime = 2.0;
    m_pParticleSystem = 0;

    m_bFirstSegment = true;
    m_fSegments = 8;
    m_fSegmentToDraw = 1;
    m_fTimeUntilNextSegment = 0.02;

    // Give this projectile a unique name so it can be referenced when needed
    m_pSceneNode = Ogre::Root::getSingletonPtr()->
                   getSceneManager(BaseApplication::GetSceneManagerName())->
                   getRootSceneNode()->createChildSceneNode();
    m_sParticleName = "Impact" + m_pSceneNode->getName();

    // Clone the material so transparency can be individually applied to each instance
    Ogre::MaterialPtr oldMaterial = Ogre::MaterialManager::getSingletonPtr()->getByName("M_Transparency");
    m_pMaterial = oldMaterial->clone("M_Transparency" + m_pSceneNode->getName(), false, "");
    Ogre::Pass* test = m_pMaterial->getTechnique(0)->getPass(0);

    m_cAmbient = m_pMaterial->getTechnique(0)->getPass(0)->getAmbient();
    m_cDiffuse = m_pMaterial->getTechnique(0)->getPass(0)->getDiffuse();
    m_cSpecular = m_pMaterial->getTechnique(0)->getPass(0)->getSpecular();
    m_cEmissive = m_pMaterial->getTechnique(0)->getPass(0)->getSelfIllumination();

    // Draw the first segment of the trail
	m_v3Source = m_pSceneNode->getOrientation() * Ogre::Vector3::UNIT_Z;
    DrawTrail();

    /*
    m_pParticleSystem = Ogre::Root::getSingletonPtr()->
                        getSceneManager(BaseApplication::GetSceneManagerName())->
                        createParticleSystem(m_sParticleName, "CannonExplosion1");
    m_pParticleSystem->setBoundsAutoUpdated(0);  // Particle effeciency needs improvements
    m_fParticleTime = 0.5f;
    m_pSceneNode->attachObject(m_pParticleSystem);
    */
}
//-----------------------------------------------------------------------------
// Use OGRE to delete model-related data before destroying this object
//-----------------------------------------------------------------------------
void SniperShot::DestroyModel()
{
    m_pSceneNode->removeAndDestroyAllChildren();
    m_pSceneNode->getParentSceneNode()->removeAndDestroyChild(m_pSceneNode->getName());
	m_pSceneNode = 0;
    m_pMaterial->unload();
    if (m_pParticleSystem)
        m_pParticleSystem = 0;
}
//-----------------------------------------------------------------------------
// Logic for drawing a trail between two given vectors
// Based on example by user "leonardoaraujo.santos" of the OGRE forums
//-----------------------------------------------------------------------------
void SniperShot::DrawTrail()
{
    if (!m_bFirstSegment)  // Destroy previous segment if this is not the first
    {
        Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->
          destroyManualObject(m_pSceneNode->getName() + "Trail");
    }
    
    // Create a trail between the enemy and the tower
    Ogre::Vector3 direction = (m_v3EndPos - m_v3StartPos)*(0.11*m_fSegmentToDraw);
	Ogre::Real distance = direction.normalise();

	// Define a vetor oriented in the Y
	Ogre::Vector3 vec_pattern(0, 5/2, 0);
	// Create a quaternion (30 de rotacao no eixo Z)
	Ogre::Quaternion rot;
	rot.FromAngleAxis(Ogre::Degree(180 / 2), Ogre::Vector3::UNIT_Z);
	
    m_pTrailModel = 0;
    m_pTrailModel = Ogre::Root::getSingletonPtr()->
                    getSceneManager(BaseApplication::GetSceneManagerName())->
                    createManualObject(m_pSceneNode->getName() + "Trail");
	m_pTrailModel->setDynamic(true);

    // Begin creating the trail object
    m_pTrailModel->begin(m_pMaterial->getName(), Ogre::RenderOperation::OT_TRIANGLE_LIST);

	for (int i = 0; i < 2; i++)
	{
		// Create a plane based on the vector
		m_pTrailModel->position(-vec_pattern.x/10, -vec_pattern.y/10, distance);
		m_pTrailModel->textureCoord(0, 0);

		m_pTrailModel->position(vec_pattern.x/10, vec_pattern.y/10, distance);  
		m_pTrailModel->textureCoord(1, 0);

        m_pTrailModel->position(-vec_pattern.x/10, -vec_pattern.y/10, 0);          
        m_pTrailModel->textureCoord(0, 1);

        m_pTrailModel->position(vec_pattern.x/10, vec_pattern.y/10, 0);         
        m_pTrailModel->textureCoord(1, 1);
            
        int offset = i * 4;
        m_pTrailModel->triangle(offset, offset+3, offset+1);
        m_pTrailModel->triangle(offset, offset+2, offset+3);

        // Multiply the quaternion by the vector to get the proper rotation
		vec_pattern = rot * vec_pattern;         
	}

	m_pTrailModel->end();  // finished creating trail
    m_pTrailModel->setCastShadows(false);
    m_pTrailModel->setLightMask(0);
    
    // Attach and rotate the trail
	m_pSceneNode->attachObject(m_pTrailModel);

    // Point in the direction of the vector formed from the start and end points
	Ogre::Quaternion quat = m_v3Source.getRotationTo(direction);
      
	// Point the trail in the direction of the line and move it to the proper position
    if (m_bFirstSegment)
    {
        m_pSceneNode->rotate(quat);
	    m_pSceneNode->translate(m_v3StartPos);
        m_bFirstSegment = false;
    } 
}
//-----------------------------------------------------------------------------
// Update the trail of the projectile and the particle
//-----------------------------------------------------------------------------
void SniperShot::Update(const Ogre::FrameEvent& p_feEvent)
{
    if (m_bParticleActive)
        m_fParticleTime -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

    if (m_fSegmentToDraw < m_fSegments)
        float test = m_fSegmentToDraw;
    
    // Draw the next segment once enough time has passed
    if (m_fTimeUntilNextSegment <= 0 && m_fSegmentToDraw < m_fSegments)
    {
        if (m_fSegmentToDraw = m_fSegments)
        {
            m_pEnemy->Attacked(m_fDamage, true, AT_SNIPER);
            
            if (m_pEnemy->GetHealth() <= 0)
            {
                m_pEnemy->GetModel()->UseTerrainHeight(true);
                m_pEnemy->GetModel()->StopMovement();

                // Play the enemy's death animation and skip directly to the end of it
                m_pEnemy->GetModel()->PlayAnimation("Die", false);
                m_pEnemy->GetModel()->AddAnimationTime(m_pEnemy->GetModel()->GetEntity()->getAnimationState("Die")->getLength() - 0.1);

                direction = (m_v3StartPos - m_pEnemy->GetLocation())/4;
                position = m_pEnemy->GetLocation() - direction;
                m_pEnemy->GetModel()->TranslateToOverTime(0.1, Ogre::Vector3(position.x, m_pEnemy->GetLocation().y, position.z));
            }


            m_pParticleSystem = Ogre::Root::getSingletonPtr()->
                                getSceneManager(BaseApplication::GetSceneManagerName())->
                                createParticleSystem(m_pSceneNode->getName() + "Impact", "ImpactLarge");
            m_pEnemy->GetSceneNode()->attachObject(m_pParticleSystem);
            m_bParticleActive = true;
            m_fParticleTime = 1.2f;
        }
        
        m_fSegmentToDraw++;
        DrawTrail();
        m_fTimeUntilNextSegment = 0.02;
    }
    else
        m_fTimeUntilNextSegment -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

    // Stop emitting the particle after 0.1 seconds have passed
    if (m_fParticleTime <= 1.1)
    {
        if (m_pParticleSystem->getEmitting())
                m_pParticleSystem->setEmitting(false);
    }

    // Start fade and stop particle
    if (m_fParticleTime <= 1.0)
    {
        float alpha = m_cAmbient.a - p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed(); 
        m_cAmbient.a = alpha;
        m_cDiffuse.a = alpha;
        m_cSpecular.a = alpha;
        m_cEmissive.a = alpha;

        m_pMaterial->getTechnique(0)->getPass(0)->setAmbient(m_cAmbient); 
        m_pMaterial->getTechnique(0)->getPass(0)->setDiffuse(m_cDiffuse);
        m_pMaterial->getTechnique(0)->getPass(0)->setSpecular(m_cSpecular);
        m_pMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(m_cEmissive);
        m_pTrailModel->setMaterialName(0, m_pMaterial->getName());
    }

    // When the trail has faded completely, flag this projectile for deletion
    if (m_fParticleTime <= 0)
    {
        Ogre::Root::getSingletonPtr()->
          getSceneManager(BaseApplication::GetSceneManagerName())->
          destroyParticleSystem(m_sParticleName);
        m_bCanDelete = true;
    }
}