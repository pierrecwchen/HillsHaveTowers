//-----------------------------------------------------------------------------
// SniperShot.cpp                                           By: Keenan Johnston
// 
// Implementation file for SniperShot.h
//-----------------------------------------------------------------------------

#include "LightningBolt.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
LightningBolt::LightningBolt(Ogre::Vector3 p_v3Loc, int p_iJump, BaseEnemy* p_pEnemy, float p_fDamage, 
                             Ogre::Vector3 p_v3Offset)
{
    m_v3StartPos = p_v3Loc + p_v3Offset;
    
    m_v3EndPos = p_pEnemy->GetLocation();
    m_fDamage = p_fDamage;
    m_pEnemy = p_pEnemy;
    m_iJumpCount = p_iJump;

    m_bCanDelete = false;
    m_bParticleActive = false;
    m_fParticleTime = 2.0;
    m_pParticleSystem = 0;

    m_fSegments = 20;
    m_iBoltCount = 0;
    m_bAttacked = false;

    // Give this projectile a unique name so it can be referenced when needed
    m_vBoltNodes.push_back(Ogre::Root::getSingletonPtr()->
                           getSceneManager(BaseApplication::GetSceneManagerName())->
                           getRootSceneNode()->createChildSceneNode());

    m_pSceneNode = Ogre::Root::getSingletonPtr()->
                   getSceneManager(BaseApplication::GetSceneManagerName())->
                   getRootSceneNode()->createChildSceneNode();
    m_sParticleName = "Impact" + m_pSceneNode->getName();

    m_fParticleTime = 0.3f;

    // Draw the first segment of the trail
	m_v3Source = m_pSceneNode->getOrientation() * Ogre::Vector3::UNIT_Z;
    DrawTrail();

    m_pParticleSystem = Ogre::Root::getSingletonPtr()->
                        getSceneManager(BaseApplication::GetSceneManagerName())->
                        createParticleSystem(m_sParticleName, "ShockwaveBlue");
    m_pParticleSystem->setBoundsAutoUpdated(0);  // Particle effeciency needs improvements
    m_pSceneNode->attachObject(m_pParticleSystem);

    // Add the initial target to the hit list
    m_vEnemiesHit.push_back(m_pEnemy);
}

LightningBolt::LightningBolt(Ogre::Vector3 p_v3Loc, Ogre::Vector3 p_v3EndLoc)
{
    m_v3StartPos = p_v3Loc;
    m_v3EndPos = p_v3EndLoc;
    m_iBoltCount = 0;
    
    m_bCanDelete = false;
    m_bParticleActive = false;
    m_fParticleTime = 2.0;
    m_pParticleSystem = 0;

    m_fSegments = 20;

    // Give this projectile a unique name so it can be referenced when needed
        m_vBoltNodes.push_back(Ogre::Root::getSingletonPtr()->
                           getSceneManager(BaseApplication::GetSceneManagerName())->
                           getRootSceneNode()->createChildSceneNode());

    m_pSceneNode = Ogre::Root::getSingletonPtr()->
                   getSceneManager(BaseApplication::GetSceneManagerName())->
                   getRootSceneNode()->createChildSceneNode();
    m_sParticleName = "Impact" + m_pSceneNode->getName();

    // Draw the first segment of the trail
	m_v3Source = m_pSceneNode->getOrientation() * Ogre::Vector3::UNIT_Z;
    DrawTrail();
}

//-----------------------------------------------------------------------------
// Use OGRE to delete model-related data before destroying this object
//-----------------------------------------------------------------------------
void LightningBolt::DestroyModel()
{
    m_pSceneNode->removeAndDestroyAllChildren();
    m_pSceneNode->getParentSceneNode()->removeAndDestroyChild(m_pSceneNode->getName());
	m_pSceneNode = 0;
    for (int i = 0; i < m_vBoltNodes.size(); ++i)
    {
        m_vBoltNodes.at(i)->removeAndDestroyAllChildren();
        m_vBoltNodes.at(i)->getParentSceneNode()->removeAndDestroyChild(m_vBoltNodes.at(i)->getName());
    }
    if (m_pParticleSystem)
        m_pParticleSystem = 0;
}
//-----------------------------------------------------------------------------
// Logic for drawing a trail between two given vectors
// Based on example by user "leonardoaraujo.santos" of the OGRE forums
//-----------------------------------------------------------------------------
void LightningBolt::DrawTrail()
{
    // Create a trail between the enemy and the tower
    Ogre::Vector3 direction = m_v3EndPos - m_v3StartPos;
	Ogre::Real distance = direction.normalise();

	// Define a vetor oriented in the Y
	Ogre::Vector3 vec_pattern(0, 5/2, 0);

	// Create a quaternion (30 de rotacao no eixo Z)
	Ogre::Quaternion rot;
	rot.FromAngleAxis(Ogre::Degree(180 / 2), Ogre::Vector3::UNIT_Z);

    m_vBoltModels.push_back(Ogre::Root::getSingletonPtr()->
                            getSceneManager(BaseApplication::GetSceneManagerName())->
                            createManualObject(m_pSceneNode->getName() + std::to_string((long long)m_iBoltCount) + "Trail"));

    // Begin creating the trail object
    m_vBoltModels.at(m_iBoltCount)->begin("Particles/Lightning", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	for (int i = 0; i < 2; i++)
	{
		// Create a plane based on the vector
        m_vBoltModels.at(m_iBoltCount)->position(-vec_pattern.x*5, -vec_pattern.y*5, distance);
		m_vBoltModels.at(m_iBoltCount)->textureCoord(0, 0);

		m_vBoltModels.at(m_iBoltCount)->position(vec_pattern.x*5, vec_pattern.y*5, distance);  
		m_vBoltModels.at(m_iBoltCount)->textureCoord(1, 0);

        m_vBoltModels.at(m_iBoltCount)->position(-vec_pattern.x*5, -vec_pattern.y*5, 0);          
        m_vBoltModels.at(m_iBoltCount)->textureCoord(0, 1);

        m_vBoltModels.at(m_iBoltCount)->position(vec_pattern.x*5, vec_pattern.y*5, 0);         
        m_vBoltModels.at(m_iBoltCount)->textureCoord(1, 1);
        
        int offset = i * 4;
        m_vBoltModels.at(m_iBoltCount)->triangle(offset, offset+3, offset+1);
        m_vBoltModels.at(m_iBoltCount)->triangle(offset, offset+2, offset+3);

        // Multiply the quaternion by the vector to get the proper rotation
		vec_pattern = rot * vec_pattern;         
	}

    m_vBoltModels.at(m_iBoltCount)->end();  // finished creating trail
    m_vBoltModels.at(m_iBoltCount)->setCastShadows(false);
    m_vBoltModels.at(m_iBoltCount)->setLightMask(0);
    
    // Attach and rotate the trail
	//m_pSceneNode->attachObject(m_pTrailModel);
    m_vBoltNodes.at(m_iBoltCount)->attachObject(m_vBoltModels.at(m_iBoltCount));

    // Point in the direction of the vector formed from the start and end points
	Ogre::Quaternion quat = m_v3Source.getRotationTo(direction);
      
	// Point the trail in the direction of the line and move it to the proper position
    m_vBoltNodes.at(m_iBoltCount)->rotate(quat);
	m_vBoltNodes.at(m_iBoltCount)->translate(m_v3StartPos);
}
//-----------------------------------------------------------------------------
// Update the trail of the projectile and the particle
//-----------------------------------------------------------------------------
void LightningBolt::Update(const Ogre::FrameEvent& p_feEvent)
{
    // Deal damage right away
    if (!m_bAttacked)
    {
        m_pEnemy->Attacked(m_fDamage, true, AT_STANDARD);
        m_bAttacked = true;
    }
    
    // Decrement timer
    m_fParticleTime -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();
    
    // Jump to a new target if possible
    if (/*m_fParticleTime <= 0.5f &&*/ m_iJumpCount > 0)
    {    
        // Check if there is an enemy in range
        if (EnemyManager::Instance()->EnemiesInRange(m_pEnemy->GetLocation(), Tile::GetDefaultSize() * 2))
        {
            std::vector<BaseEnemy*> enemies = EnemyManager::Instance()->GetEnemiesInRange(m_pEnemy->GetLocation(), Tile::GetDefaultSize() * 2);
            for (int i = 0; i < enemies.size(); ++i)
            {
                bool attack = true;
                
                // Check the targets
                for (int j = 0; j < m_vEnemiesHit.size(); ++j)
                    if (enemies.at(i) == m_vEnemiesHit.at(j))
                        attack = false;

                // If the target has not been hit yet, attack it
                if (attack)
                {
                    // Increment the bolt count and decrement jump count
                    m_iBoltCount++;
                    //m_iJumpCount--;

                    // Create a new scene node
                    m_vBoltNodes.push_back(Ogre::Root::getSingletonPtr()->
                                           getSceneManager(BaseApplication::GetSceneManagerName())->
                                           getRootSceneNode()->createChildSceneNode());

                    // Set the new start and end locations
                    m_v3StartPos = m_v3EndPos;
                    m_v3EndPos = enemies.at(i)->GetLocation();

                    // Add this enemy to the hit list
                    m_vEnemiesHit.push_back(enemies.at(i));

                    // Reset the attack flag
                    m_bAttacked = false;

                    // Draw the next trail segment
                    DrawTrail();
                }
            }
        }
    }
    else if (m_fParticleTime <= 0.2f)
    {
        if (m_pParticleSystem->getEmitting())
                m_pParticleSystem->setEmitting(false);
    }

    // Flag this projectile for deletion when the attacks are finished
    if (m_fParticleTime <= 0)
    {
        Ogre::Root::getSingletonPtr()->
          getSceneManager(BaseApplication::GetSceneManagerName())->
          destroyParticleSystem(m_sParticleName);
        m_bCanDelete = true;
    }
}