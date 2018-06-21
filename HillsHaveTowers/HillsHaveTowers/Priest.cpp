//-----------------------------------------------------------------------------
// Priest.cpp                                               By: Keenan Johnston
// 
// Implementation file for Priest.h
//-----------------------------------------------------------------------------

#include "Priest.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Priest::Priest(std::list<Tile> p_lShortestPath, float p_fXOffset, float p_fZOffset)
{
    // Set up priest stats
	m_iCurrentHealth = m_iMaxHealth = 5000;
	m_iVillagersCarried = 0;
	m_fDmgReduction = 0;
	m_fWalkSpeed = 1;
    m_fHealDelay = 1;

    m_bHasArmor = false;

    m_bIdentified = false;

    m_fScreamPitch = 1.35f;

    m_cLoot = Currency(50,0,0,0);

    m_fCircleSize = 10.0f;
    m_fCircleOffset = 0.0f;

    // Set up path variables
    m_lCurrentPath = p_lShortestPath;
    m_lOriginalPath = p_lShortestPath;

    p_lShortestPath.reverse();
    m_lExitPath = p_lShortestPath;

    m_tTargetTile = m_lCurrentPath.front();
    m_bIsOnCorrectPath = true;
    m_bIsLeaving = false;

    m_fTimeSinceEscape = 0.0f;

    m_bCanDelete = false;
    
    // Create the visual representation of the enemy and play its walk animation
    m_pModel = new Model("priest.mesh");
    m_pModel->SetAnimationBlending(true);
	m_pModel->PlayAnimation("Walk", true);
    m_pModel->Scale(Ogre::Vector3(0.7f));

    m_iHeight = Terrain::GetHeight(m_tTargetTile.GetLocation().x, m_tTargetTile.GetLocation().z) - 23;
    m_pModel->SetHeightAdjustment(m_iHeight);

    if(p_fXOffset == 1000 && p_fZOffset == 1000)
    {
        m_fXOffset = (((float)(rand() % 1001) / 1000) - 0.5f) * Tile::GetDefaultSize() / 1.1;
        m_fZOffset = (((float)(rand() % 1001) / 1000) - 0.5f) * Tile::GetDefaultSize() / 1.1;
    }
    else
    {
        m_fXOffset = p_fXOffset;
        m_fZOffset = p_fZOffset;
    }

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

    m_fAnimationSpeed = 1.5f;
    m_pModel->SetAnimationSpeed(m_fAnimationSpeed);

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
	// line 100
	pCube->position(10, 0, 0);
	pCube->position(10, 0, 10);
	// line 1010
	pCube->position(0, 10, 0);
	pCube->position(0, 10, 10);
	// line 102
	pCube->position(10, 10, 0);
	pCube->position(10, 10, 10);

	pCube->end();
    pCube->setCastShadows(false);
    pCube->setQueryFlags(QF_ENEMIES);

    m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
    m_pBoundingBox->attachObject(pCube);
    m_pBoundingBox->setVisible(false);
    m_pModel->GetSceneNode()->getChild(m_pModel->GetSceneNode()->getName() + "Box")->translate(Ogre::Vector3(-6,-9,-6));
    m_pModel->GetSceneNode()->getChild(m_pModel->GetSceneNode()->getName() + "Box")->scale(Ogre::Vector3(1.2,1.7,1.1));

    // Set up heal particle
    m_pHealParticle = Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->
                      createParticleSystem(m_pModel->GetSceneNode()->getName() + "Heal", "Heal2");
    m_pHealParticle->setEmitting(false);
    m_pModel->GetSceneNode()->attachObject(m_pHealParticle);
    
    // Default to walk state
    m_eState = WalkState;

	pCube = 0;
}

//-----------------------------------------------------------------------------
// Overloaded update loop to handle healing event
//-----------------------------------------------------------------------------
void Priest::Update(const Ogre::FrameEvent& p_feEvent)
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

    // Heal logic
    if (m_eState == WalkState)
    {
        m_fHealDelay -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();
        
        // Check if a second has elapsed and if it has, perform a heal
        if (m_fHealDelay <= 0)
        {
            m_fHealDelay = 1;
            Hills::EventManager::AddEvent(Hills::GameEvent(Hills::GE_Heal, 
                                                    m_pModel->GetLocation(), 100, 
                                                    m_pModel->GetSceneNode()->getName()));
        }
    }

	// If an enemy has no health left, put them in the death state
    if (m_iCurrentHealth <= 0 && m_eState != DeadState)
    {
        // Regardless of the type of attack, we will always go to the death state
        // and award loot when the enemy is killed
        m_eState = DeadState;
        MainCurrency::Add(m_cLoot);

        // Check how the enemy was killed, and play a special effect if appropriate
        if (m_eKillingAttack == AT_FIRE)
        {
            m_pModel->SetAnimationSpeed(1.0f);
            m_pModel->StopMovement();
            m_pModel->PlayAnimation("Die", false);
            DieByFire();
        }
        else if (m_eKillingAttack == AT_SNIPER)
        {
            m_fDeathTimeLeft = 1.5f;
        }
        else if (m_eKillingAttack == AT_STANDARD)
        {
            m_fDeathTimeLeft = 1.0f;
            m_pModel->SetAnimationSpeed(1.0f);
            m_pModel->StopMovement();
            m_pModel->PlayAnimation("Die", false);
        }

        PlayDeathSound();

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
    else if (m_eState == DeadState)  // Play animation until finished then destroy this instance
    {

        if (m_eKillingAttack == AT_FIRE)
        {
            m_fDeathTimeLeft -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

            // Stop the particle effect when the death timer is 0
            if (m_fDeathTimeLeft <= 0.0f)
                m_pParticleSystem->setEmitting(false);
            // And start it when there are 2 seconds left
            else if (m_fDeathTimeLeft <= 2.5f)
            {
                m_bParticleStarted = true;
                m_pParticleSystem->setEmitting(true);
            }
            
            // Check if all the smoke particles have dissapated and the death timer is 0
            if (m_fDeathTimeLeft <= 0.0f && m_pParticleSystem->getNumParticles() == 0)
                m_bCanDelete = true;
        }
        else if (m_eKillingAttack == AT_SNIPER)
        {
            // Countdown as soon as the killing blow lands
            m_fDeathTimeLeft -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

            if (m_fDeathTimeLeft <= 0.0f) 
                m_bCanDelete = true;
        }
        else if (m_eKillingAttack == AT_STANDARD)
        {
            // Countdown after the death animation has finished
            if (m_pModel->HasAnimationEnded())
                m_fDeathTimeLeft -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

            if (m_fDeathTimeLeft <= 0.0f)
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
                if(m_lExitPath.size() == 0)
                {
                    if(m_pModel->GetRotation() < 45 || m_pModel->GetRotation() >= 315)
                    {
                        m_pModel->RotateTo(0);
                    }
                    else if(m_pModel->GetRotation() >= 45 && m_pModel->GetRotation() < 135)
                    {
                        m_pModel->RotateTo(90);
                    }
                    else if(m_pModel->GetRotation() >= 135 && m_pModel->GetRotation() < 225)
                    {
                        m_pModel->RotateTo(180);
                    }
                    else if(m_pModel->GetRotation() >= 225 && m_pModel->GetRotation() < 315)
                    {
                        m_pModel->RotateTo(270);
                    }

                    m_eState = EscapeState;
                }
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
    else if(m_eState == EscapeState)
    {
        Ogre::Camera* tempCam = Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->getCamera("PlayerCam");

        if(tempCam->isVisible(m_pModel->GetEntity()->getBoundingBox()))
        {
            m_bCanDelete = true;
        }
        else
        {
            m_pModel->MoveForward(m_fWalkSpeed * m_fSpeedModifier * Tile::GetDefaultSize() * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed());
        }
    }

    // Update the model
    m_pModel->Update(p_feEvent);
}

//-----------------------------------------------------------------------------
// Special animation to perform if an enemy is killed by a fire attack
//-----------------------------------------------------------------------------
void Priest::DieByFire()
{
    // If they were killed by a fire attack, darken their model and prepare a smoke particle
    m_fDeathTimeLeft = 2.05f;
    m_pModel->SetColour(Ogre::ColourValue(0,0,0));
    m_pParticleSystem = Ogre::Root::getSingletonPtr()->
                        getSceneManager(BaseApplication::GetSceneManagerName())->
                        createParticleSystem(m_pModel->GetSceneNode()->getName() + "Smoke", "SmokeX");
    m_pParticleSystem->setEmitting(false);
    m_pEffectNode = m_pModel->GetSceneNode()->createChildSceneNode();
    m_pEffectNode->translate(0, -5, 6);
    m_pEffectNode->attachObject(m_pParticleSystem);
}