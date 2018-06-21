//-----------------------------------------------------------------------------
// Demon.cpp                                                By: Keenan Johnston
// 
// Implementation file for Demon.h
//-----------------------------------------------------------------------------

#include "Demon.h"
#include "SoundManager.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Demon::Demon(std::list<Tile> p_lShortestPath, float p_fXOffset, float p_fZOffset)
{
    // Set up demon stats
	m_iCurrentHealth = m_iMaxHealth = 4000;
	m_iVillagersCarried = 1;
	m_fDmgReduction = 0.15f;
	m_fWalkSpeed = 1;
    m_fAttackTime = 4.0f;

    m_bHasArmor = false;

    m_bAttackEventFired = false;

    m_iFireChannel = -1;

    m_fScreamPitch = 0.2f;

    m_cLoot = Currency(20,0,0,0);

    m_fCircleSize = 18.0f;
    m_fCircleOffset = 0.0f;

    // Set up path variables
    m_lCurrentPath = p_lShortestPath;
    m_lOriginalPath = p_lShortestPath;

    p_lShortestPath.reverse();
    m_lExitPath = p_lShortestPath;

    m_tTargetTile = m_lCurrentPath.front();
    m_bIsOnCorrectPath = true;
    m_bIsLeaving = false;

    m_bCanDelete = false;
    
    // Create the visual representation of the enemy and play its walk animation
    m_pModel = new Model("demon.mesh");
    m_pModel->SetAnimationBlending(true);
	m_pModel->PlayAnimation("Walk", true);
    m_pModel->Scale(Ogre::Vector3(2.0f));

    // Name to append to particles
    std::string sName = m_pModel->GetSceneNode()->getName();

    // Attach eye particles
    m_pModel->GetEntity()->attachObjectToBone("skull", Ogre::Root::getSingletonPtr()->
                                                       getSceneManager(BaseApplication::GetSceneManagerName())->
                                                       createParticleSystem("EyeFire1" + sName, "DemonFireTiny"),
                                                       Ogre::Quaternion::IDENTITY, Ogre::Vector3(0.3,1,0.85));
    m_pModel->GetEntity()->attachObjectToBone("skull", Ogre::Root::getSingletonPtr()->
                                                       getSceneManager(BaseApplication::GetSceneManagerName())->
                                                       createParticleSystem("EyeFire2" + sName, "DemonFireTiny"),
                                                       Ogre::Quaternion::IDENTITY, Ogre::Vector3(-0.3,1,0.85));

    // Attach hand particles
    m_pModel->GetEntity()->attachObjectToBone("fingers_l", Ogre::Root::getSingletonPtr()->
                                                       getSceneManager(BaseApplication::GetSceneManagerName())->
                                                       createParticleSystem("HandFireLeft" + sName, "DemonFireHands"));
    m_pModel->GetEntity()->attachObjectToBone("fingers_r", Ogre::Root::getSingletonPtr()->
                                                       getSceneManager(BaseApplication::GetSceneManagerName())->
                                                       createParticleSystem("HandFireRight" + sName, "DemonFireHands"));
    Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->
      getParticleSystem("HandFireLeft" + sName)->setEmitting(false);
    Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->
      getParticleSystem("HandFireRight" + sName)->setEmitting(false);

    // Particles for death sequence
    /*
    m_pFire = Ogre::Root::getSingletonPtr()->
              getSceneManager(BaseApplication::GetSceneManagerName())->
              createParticleSystem("ChestFire" + sName, "DemonFireLarge");
    m_pModel->GetEntity()->attachObjectToBone("spine_2", m_pFire);
    m_pFire->setEmitting(false);
    */

    m_pGroundFire = Ogre::Root::getSingletonPtr()->
                    getSceneManager(BaseApplication::GetSceneManagerName())->
                    createParticleSystem("GroundFire" + sName, "DemonFireLargeArea");
    //m_pModel->GetEntity()->attachObjectToBone("spine_2", m_pGroundFire);
    //m_pModel->GetSceneNode()->attachObject(m_pGroundFire);
    //m_pGroundFire->setEmitting(false);

    // Create particles for explosion
    m_pFireSparks = Ogre::Root::getSingletonPtr()->
                    getSceneManager(BaseApplication::GetSceneManagerName())->
                    createParticleSystem("FireSparks" + sName, "FireSparks");
    m_pFireSparks->setEmitting(false);
    m_pModel->GetEntity()->attachObjectToBone("fingers_l", m_pFireSparks);

    m_pFireWave = Ogre::Root::getSingletonPtr()->
                  getSceneManager(BaseApplication::GetSceneManagerName())->
                  createParticleSystem("FireWave" + sName, "FireWave");
    m_pFireWave->setEmitting(false);
    m_pModel->GetEntity()->attachObjectToBone("fingers_l", m_pFireWave);

    m_pFireExplosion = Ogre::Root::getSingletonPtr()->
                       getSceneManager(BaseApplication::GetSceneManagerName())->
                       createParticleSystem("FireExplosion" + sName, "FireExplosion");
    m_pFireExplosion->setEmitting(false);
    m_pModel->GetEntity()->attachObjectToBone("fingers_l", m_pFireExplosion);

    m_pFireImpact = Ogre::Root::getSingletonPtr()->
                       getSceneManager(BaseApplication::GetSceneManagerName())->
                       createParticleSystem("FireImpact" + sName, "FireImpact2");
    m_pFireImpact->setEmitting(false);
    m_pModel->GetEntity()->attachObjectToBone("fingers_l", m_pFireImpact);

    m_pFireFlash = Ogre::Root::getSingletonPtr()->
                       getSceneManager(BaseApplication::GetSceneManagerName())->
                       createParticleSystem("FireFlash" + sName, "FireFlash");
    m_pFireFlash->setEmitting(false);
    m_pModel->GetEntity()->attachObjectToBone("fingers_l", m_pFireFlash);

    m_iHeight = Terrain::GetHeight(m_tTargetTile.GetLocation().x, m_tTargetTile.GetLocation().z) - 10;
    m_pModel->SetHeightAdjustment(m_iHeight);

    m_fXOffset = 0;
    m_fZOffset = 0;

    Ogre::Vector3 position = m_lCurrentPath.front().GetLocation();
    position.y += m_iHeight;
    position.x += m_fXOffset;
    position.z += m_fZOffset;
    m_pModel->Translate(position);

    if(m_lCurrentPath.front().GetGridX() == 0)
    {
        m_pModel->Translate(Ogre::Vector3(-Tile::GetDefaultSize() * 6, 0, 0));
    }
    else if (m_lCurrentPath.front().GetGridY() == 0)
    {
        m_pModel->Translate(Ogre::Vector3(0, 0, -Tile::GetDefaultSize() * 6));
    }
    else if (m_lCurrentPath.front().GetGridX() == TileSet::GetWidth() - 1)
    {
        m_pModel->Translate(Ogre::Vector3(Tile::GetDefaultSize() * 6, 0, 0));
    }
    else if (m_lCurrentPath.front().GetGridY() == TileSet::GetLength() - 1)
    {
        m_pModel->Translate(Ogre::Vector3(0, 0, Tile::GetDefaultSize() * 6));
    }

    m_tTargetTile = m_lCurrentPath.front();

    m_pModel->Rotate(GetAngleToNextTile());

    m_fAnimationSpeed = 0.5f;
    m_pModel->SetAnimationSpeed(m_fAnimationSpeed);
    m_pModel->SetAnimationBlending(true);

    // Set up custom bounding box
    Ogre::ManualObject* pCube = new Ogre::ManualObject(m_pModel->GetSceneNode()->getName());
	pCube->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
	// line 1
	pCube->position(0, 0, 0);
	pCube->position(10, 0, 0);
	// line 2
	pCube->position(10, 10, 0);
	pCube->position(0, 10, 0);
	// line 3
	pCube->position(0, 0, 0);
	pCube->position(0, 10, 0);
	// line 4
	pCube->position(10, 0, 0);
	pCube->position(10, 10, 0);
	// line 5
	pCube->position(0, 0, 10);
	pCube->position(10, 0, 10);
	// line 6
	pCube->position(0, 10, 10);
	pCube->position(10, 10, 10);
	// line 7
	pCube->position(0, 0, 10);
	pCube->position(0, 10, 10);
	// line 8
	pCube->position(10, 0, 10);
	pCube->position(10, 10, 10);
	// line 9
	pCube->position(0, 0, 0);
	pCube->position(0, 0, 10);
	// line 10
	pCube->position(10, 0, 0);
	pCube->position(10, 0, 10);
	// line 11
	pCube->position(0, 10, 0);
	pCube->position(0, 10, 10);
	// line 12
	pCube->position(10, 10, 0);
	pCube->position(10, 10, 10);

	pCube->end();
    pCube->setCastShadows(false);
    pCube->setQueryFlags(QF_ENEMIES);

    m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
    m_pBoundingBox->attachObject(pCube);
    m_pBoundingBox->setVisible(false);
    m_pModel->GetSceneNode()->getChild(m_pModel->GetSceneNode()->getName() + "Box")->translate(Ogre::Vector3(-5,-7,-5));
    m_pModel->GetSceneNode()->getChild(m_pModel->GetSceneNode()->getName() + "Box")->scale(Ogre::Vector3(1,1.5,1));

    // Set up heal particle
    m_pHealParticle = Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->
                      createParticleSystem(m_pModel->GetSceneNode()->getName() + "Heal", "Heal2");
    m_pHealParticle->setEmitting(false);
    m_pModel->GetSceneNode()->attachObject(m_pHealParticle);

    m_bAttackPlaying = false;
    
    // Default to walk state
    m_eState = ES_IDLE;

    m_fAnimationTime = 0;

    // Get the scene camera so we can shake it during the attack animation
    m_pCamera = Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->
                getCamera("PlayerCam");
    m_bCameraShaking = false;
    m_fShakeTime = 0;
    m_fFrequency = 15;
    m_fAmplitude = 6;

	pCube = 0;
}

//-----------------------------------------------------------------------------
// Special animation to perform if an enemy is killed by a fire attack
//-----------------------------------------------------------------------------
void Demon::DieByFire()
{
}

void Demon::PlayEscapeSound()
{
}

//-----------------------------------------------------------------------------
// Overloaded update loop to handle attack event
//-----------------------------------------------------------------------------
void Demon::Update(const Ogre::FrameEvent& p_feEvent)
{
    // Check status of heal particle if it is active
    if (m_bHealed)
    {
        m_fHealEmitter -= p_feEvent.timeSinceLastFrame;
        
        if (m_fHealEmitter <= 0)
        {
            m_pHealParticle->setEmitting(false);
            m_bHealed = false;
        }
    }
    
    if(!m_bIsOnCorrectPath)
    {
        SetShortestPath(m_lReceivedPath);
    }

    // Check if this enemy is currently affected by a slow effect
    if (m_bSlowed)
    {
        m_fSlowTime -= p_feEvent.timeSinceLastFrame;

        // If the slow is over, return to normal speed
        if (m_fSlowTime <= 0)
        {
            m_fSpeedModifier = 1.0f;
            m_pModel->SetAnimationSpeed(1.0f);
            m_bSlowed = false;
        }
    }

    // Continue shaking the camera if the amplitude is above 0
    if (m_bCameraShaking)
    {
        m_pCamera->setPosition(m_vCameraPos + Ogre::Vector3(0, sin(m_fShakeTime*m_fFrequency)*m_fAmplitude, 0));
        m_fShakeTime += p_feEvent.timeSinceLastFrame;
        m_fAmplitude -= p_feEvent.timeSinceLastFrame*4;

        // Stop the shake calculations after the attack state is finished, otherwise
        // it will happen again with the current amplitude reduction
        if (m_fAmplitude <= 0)
            m_bCameraShaking = false;
    }

    if (m_eState == ES_IDLE)
    {
        // Currently nothing needs to be done here, this state is for the
        // boss intro cutscene and we manually handle his animations and
        // actions in the cutscene class.
    }

    // Play attack animation and create proper particles
    if (m_eState == ES_ATTACK)
    {
        m_fAnimationTime += p_feEvent.timeSinceLastFrame;

        if (!m_bAttackPlaying)
        {
            m_pModel->PlayAnimation("Attack", false);
            //m_fAnimationTime = 2.0833f / m_fAnimationSpeed;
            m_fAnimationTime = 0;

            m_bAttackPlaying = true;

            // Turn hand fire on
            Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->
              getParticleSystem("HandFireLeft" + m_pModel->GetSceneNode()->getName())->setEmitting(true);
            Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->
              getParticleSystem("HandFireRight" + m_pModel->GetSceneNode()->getName())->setEmitting(true);
        }

        // Turn particles on and off at the proper times and shake the camera when the
        // explosion goes off
        if (m_fAnimationTime >= 2.0833f / m_fAnimationSpeed)
        {
            m_bAttackPlaying = false;
            m_pModel->PlayAnimation("Walk", true);
            m_eState = WalkState;

            // Turn hand fire off
            Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->
              getParticleSystem("HandFireLeft" + m_pModel->GetSceneNode()->getName())->setEmitting(false);
            Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->
              getParticleSystem("HandFireRight" + m_pModel->GetSceneNode()->getName())->setEmitting(false);
        }
        //if (m_pModel->GetAnimationState()->getTimePosition() >= 1.2708f)
        else if (m_fAnimationTime >= 1.2708f / m_fAnimationSpeed)
        {
            m_pFireSparks->setEmitting(false);
            m_pFireWave->setEmitting(false);
            m_pFireExplosion->setEmitting(false);
            m_pFireFlash->setEmitting(false);
        }
        //else if (m_pModel->GetAnimationState()->getTimePosition() >= 1.2117f)
        else if (m_fAnimationTime >= 1.2117f / m_fAnimationSpeed)
        {
            m_pFireImpact->setEmitting(false);
        }
        //else if (m_pModel->GetAnimationState()->getTimePosition() >= 1.1017f)
        else if (m_fAnimationTime >= 1.1017f / m_fAnimationSpeed)
        {
            m_pFireImpact->setEmitting(true);
        }
        //else if (m_pModel->GetAnimationState()->getTimePosition() >= 1.0417f)
        else if (m_fAnimationTime >= 1.0417f / m_fAnimationSpeed)
        {
            m_pFireSparks->setEmitting(true);
            m_pFireWave->setEmitting(true);
            m_pFireExplosion->setEmitting(true);
            m_pFireFlash->setEmitting(true);

            if (!m_bAttackEventFired)
            {
                SoundManager::Play3DSound("bossAttack.wav", m_pModel->GetLocation());
                Hills::EventManager::AddEvent(Hills::GameEvent(Hills::GE_FreezeAttack, 
                                                               m_pModel->GetLocation(),
                                                               Tile::GetDefaultSize() * 3));
                m_bAttackEventFired = true;
            }
            if (!m_bCameraShaking)
            {
                m_vCameraPos = m_pCamera->getPosition();
                m_bCameraShaking = true;
                m_fShakeTime = 0;
                m_fAmplitude = 6;
            }
        }

        //if (m_pModel->HasAnimationEnded())
        if (m_fAnimationTime >= 2.0833f / m_fAnimationSpeed)
        {
            m_bAttackPlaying = false;
            m_pModel->PlayAnimation("Walk", true);
            m_eState = WalkState;

            // Turn hand fire off
            Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->
              getParticleSystem("HandFireLeft" + m_pModel->GetSceneNode()->getName())->setEmitting(false);
            Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->
              getParticleSystem("HandFireRight" + m_pModel->GetSceneNode()->getName())->setEmitting(false);
        }
    }

    // Go to attack state if needed
    if (m_eState == WalkState)
    {
        m_fAttackTime -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();
        
        // Check if a second has elapsed and if it has, go into the attack state
        if (m_fAttackTime <= 0)
        {
            m_fAttackTime = 4.0f;
            m_eState = ES_ATTACK;
            m_bAttackEventFired = false;
        }
    }

	// If an enemy has no health left, put them in the death state
    if (m_iCurrentHealth <= 0 && m_eState != DeadState)
    {
        // Regardless of the type of attack, we will always go to the death state
        // and award loot when the enemy is killed
        m_eState = DeadState;
        MainCurrency::Add(m_cLoot);

        // Unlike the other enemies, the boss has a single death sequence
        //m_pMaterial = Ogre::MaterialManager::getSingleton().getByName("demon");

        // Turn on fire particle
        m_pFireNode = Ogre::Root::getSingletonPtr()->
                      getSceneManager(BaseApplication::GetSceneManagerName())->
                      getRootSceneNode()->createChildSceneNode("Ground_Fire" + m_pModel->GetSceneNode()->getName());
        m_pFireNode->translate(m_pModel->GetSceneNode()->getPosition().x,
                               Terrain::GetHeight(m_pModel->GetSceneNode()->getPosition().x,
                                                  m_pModel->GetSceneNode()->getPosition().z),
                               m_pModel->GetSceneNode()->getPosition().z);
        m_pFireNode->attachObject(m_pGroundFire);
        m_pGroundFire->setEmitting(false);

        //m_pFire->setEmitting(true);

        m_fDeathTimeLeft = 5.0f;
        m_pModel->SetAnimationSpeed(0.4f);
        m_pModel->StopMovement();
        m_pModel->PlayAnimation("Die", false);

        SoundManager::Play3DSound("bossDeath.wav", m_pModel->GetLocation());

		/*****************************************************************************
		*	Cannot be selected and cannot recieve events in dead state. (By Pierre)
		******************************************************************************/
		if(m_bIsSelectable)
		{
			SetSelectable(false);
		}

		if(m_bIsAttached)
		{
			Deattach();
		}
        /*****************************************************************************/
    }
    // Play the death animation, then start a fire particle which engulfs the body as
    // it dissapears and fades out
    else if (m_eState == DeadState)
    {
        m_fDeathTimeLeft -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

        if (m_fDeathTimeLeft <= 0)
        {   
            m_pModel->Scale(Ogre::Vector3(1 - p_feEvent.timeSinceLastFrame/3));
            m_pModel->TranslateTo(Ogre::Vector3(m_pModel->GetSceneNode()->getPosition().x, 
                m_pModel->GetSceneNode()->getPosition().y - p_feEvent.timeSinceLastFrame*3,
                m_pModel->GetSceneNode()->getPosition().z));

            if(m_iFireChannel == -1)
            {
                m_iFireChannel = SoundManager::Play3DLoopedSound("bossSummonIntenseFire.wav", m_pModel->GetLocation());
            }

            m_pGroundFire->setEmitting(true);
        }

        if (m_fDeathTimeLeft <= -5)
        {
            SoundManager::StopSoundAtChannel(m_iFireChannel);
            m_pGroundFire->setEmitting(false);
            m_bCanDelete = true;
        }
    }

    //ALL PATHFINDING HERE IS IDENTICAL TO THAT IN BaseEnemy.cpp
    else if (m_eState == WalkState && !m_bCanDelete)  // Continue walking along the path
    {
        if(!m_bIsLeaving)
        {
            float angle = GetAngleToNextTile();

            float rotation = GetAngleDifference(m_pModel->GetRotation(), angle) * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() * 2.5 * m_fWalkSpeed * m_fSpeedModifier;
            
            if(rotation > 0.001 || rotation < -0.001)
                m_pModel->Rotate(rotation);

            
            m_pModel->MoveForward(m_fWalkSpeed * m_fSpeedModifier * Tile::GetDefaultSize() * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed());

            if(m_lCurrentPath.size() == 0)
            {
                m_bIsLeaving = true;
                m_pModel->Rotate(180);
            }
            else
            {
               if(Tile(m_pModel->GetLocation()) == m_tTargetTile)
               {
                   m_lCurrentPath.pop_front();
               }
               
               if(m_lCurrentPath.size() != 0)
               {
                   m_tTargetTile = m_lCurrentPath.front();
               }
            }
        }
        else
        {
            float angle = GetAngleToNextTile();

            float rotation = GetAngleDifference(m_pModel->GetRotation(), angle) * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() * 2.5 * m_fWalkSpeed * m_fSpeedModifier;
            
            if(rotation > 0.001 || rotation < -0.001)
                m_pModel->Rotate(rotation);

            m_pModel->MoveForward(m_fWalkSpeed * m_fSpeedModifier * Tile::GetDefaultSize() * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed());

            if(m_lExitPath.size() == 0)
            {
                m_bCanDelete = true;
            }
            else
            {
               if(Tile(m_pModel->GetLocation()) == m_tTargetTile)
               {
                   m_lExitPath.pop_front();
               }
               
               if(m_lExitPath.size() != 0)
               {
                   m_tTargetTile = m_lExitPath.front();
               }
            }
        }
    }

    // Update the model
    m_pModel->Update(p_feEvent);
}