//-----------------------------------------------------------------------------
// BaseEnemy.cpp                                            By: Keenan Johnston
// 
// Implementation file for BaseEnemy.h
//-----------------------------------------------------------------------------

#include "BaseEnemy.h"
#include "SoundManager.h"

//-----------------------------------------------------------------------------
// Default constructor: used to initialize some values
//-----------------------------------------------------------------------------
BaseEnemy::BaseEnemy()
{
    m_iCurrentVillagersCarried = 0;
    m_fSpeedModifier = 1.0f;
    m_fSlowTime = 0.0f;
    m_bSlowed = false;
    m_bParticleStarted = false;
    m_fHealEmitter = 0;
    m_bHealed = false;
}
//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
BaseEnemy::BaseEnemy(int p_iHp, int p_iCarry, float p_fArmour, float p_fSpeed,
                     std::string p_sMesh, Ogre::SceneManager* p_smManager,
                     std::list<Tile> p_lShortestPath)
{
	// Set up enemy stats
	m_iCurrentHealth = p_iHp;
	m_iVillagersCarried = p_iCarry;
	m_fDmgReduction = p_fArmour;
	m_fWalkSpeed = p_fSpeed;

    // Set up path
    m_lCurrentPath = p_lShortestPath;
    m_lOriginalPath = p_lShortestPath;

    p_lShortestPath.reverse();
    m_lExitPath = p_lShortestPath;

    m_tTargetTile = m_lCurrentPath.front();
    m_bIsOnCorrectPath = true;
    m_bIsLeaving = false;

    m_bCanDelete = false;

    // Create the visual representation of the enemy and play its walk animation
    m_pModel = new Model(p_sMesh);
    m_pModel->Translate(Ogre::Vector3(m_tTargetTile.GetGridX() * 100 + m_fXOffset, 
                                      -68, m_tTargetTile.GetGridY() * 100 + m_fZOffset));
    m_pModel->Scale(Ogre::Vector3(5));
    m_pModel->SetAnimationBlending(true);
	m_pModel->PlayAnimation("Walk", true);

    // Default to walk state
    m_eState = WalkState;
}
//-----------------------------------------------------------------------------
// Destructor: clean up any allocated memory
//-----------------------------------------------------------------------------
BaseEnemy::~BaseEnemy()
{
	m_lOriginalPath.clear();
	m_lCurrentPath.clear();
	m_lExitPath.clear();
	m_lReceivedPath.clear();
	
	m_pBoundingBox = 0;
    
	m_pModel->CleanUp();
	delete m_pModel;
}
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
// NICK: Get a list of the tiles that form the path for movement
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
void BaseEnemy::SetShortestPath(std::list<Tile> p_lPath)
{
    //We set our shortest path to "the received path" indicating that we're simply holding onto
    // it for now
    m_lReceivedPath = p_lPath;

    //If we're on this newly received path
    if(IsOnNewPath())
    {
        m_lOriginalPath = p_lPath;
        //We'll set a flag indicating that we are indeed on the new path
        m_bIsOnCorrectPath = true;

        //We don't know which tile on the path we're on, we're not matched to one yet, so we set
        // this to false
        bool matchFound = false;

        //If we're heading to the village
        if(!m_bIsLeaving)
        {
            //While we haven't found our tile
            while(!matchFound)
            {
                //We check if this tile is our tile
                if(!(m_tTargetTile == m_lReceivedPath.front()))
                {
                    //If not, toss it
                    m_lReceivedPath.pop_front();
                }
                else
                {
                    matchFound = true;
                }
            }
            
            //Once we found our tile, we start from there and continue on
            m_lCurrentPath = m_lReceivedPath;
        }

        //We also need to alter our exit path as well. This will always need to happen
        // whereas we only need to alter the entrance path if we're actually entering.
        m_lReceivedPath = p_lPath;
        
        //We flip our path and continue as we had above
        m_lReceivedPath.reverse();
        matchFound = false;

        if(!m_bIsLeaving)
        {
            m_lExitPath = m_lReceivedPath;
        }
        else
        {
            while(!matchFound)
            {
                if(!(m_tTargetTile == m_lReceivedPath.front()))
                {
                    m_lReceivedPath.pop_front();
                }
                else
                {
                    matchFound = true;
                }
            }

            m_lExitPath = m_lReceivedPath;
        }
        m_lReceivedPath.clear();
    }
    //If we aren't actually on our new path at the moment, then we flag ourselves as off 
    // the path and we just continue with the old one, saving the pathfinding for later
    else
    {
        m_bIsOnCorrectPath = false;
    }
}
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
// NICK: Check if the tile the enemy is currently on is part of the new path
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
bool BaseEnemy::IsOnNewPath()
{
    std::list<Tile>::iterator it;

    for(it = m_lReceivedPath.begin(); it != m_lReceivedPath.end(); it++)
    {
        if(m_tTargetTile == *it) return true;
    }

    return false;
}
//-----------------------------------------------------------------------------
// Enemy is attacked is attacked by a tower
//-----------------------------------------------------------------------------
void BaseEnemy::Attacked(float power, bool pierce, AttackTypes p_eAttackType)
{
    // Play some sort of animation or effect depending on type of attack?
	if (pierce)
        m_iCurrentHealth -= power;
	else
        m_iCurrentHealth -= power * (1-m_fDmgReduction);

    if (m_iCurrentHealth <= 0)
        m_eKillingAttack = p_eAttackType;
}
//-----------------------------------------------------------------------------
// Enemy is healed by a priest
//-----------------------------------------------------------------------------
void BaseEnemy::Healed(int p_iHealth)
{
    m_iCurrentHealth += p_iHealth;
    if (m_iCurrentHealth >= m_iMaxHealth)
        m_iCurrentHealth = m_iMaxHealth;  // don't heal past max health
}
//-----------------------------------------------------------------------------
// Determine if this enemy is (still) within range of a given tower
//-----------------------------------------------------------------------------
bool BaseEnemy::InRange(Ogre::Vector3 p_v3Loc, float p_fRange, float p_fBlindness)
{
    bool isInRange = false;
    
    // Do a circular check
    Ogre::Vector3 loc = m_pModel->GetLocation();
	float inRange = sqrt(pow(loc.x - p_v3Loc.x, 2) + pow(loc.z - p_v3Loc.z, 2));

    if(!m_bIdentified)
    {
        inRange /= p_fBlindness;
    }
    
    if (inRange <= p_fRange)
        isInRange = true;
        
    return isInRange;
}
//-----------------------------------------------------------------------------
// Use OGRE to delete model-related data before destroying this object
//-----------------------------------------------------------------------------
void BaseEnemy::DestroyModel()
{
    m_pModel->CleanUp();
}
//-----------------------------------------------------------------------------
// Update this object
//-----------------------------------------------------------------------------
void BaseEnemy::Update(const Ogre::FrameEvent& p_feEvent)
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
    
    // Check if this enemy is currently affected by a slowing effect
    if (m_bSlowed)
    {
        m_fSlowTime -= p_feEvent.timeSinceLastFrame;

        // If the slow is over, return to normal speed
        if (m_fSlowTime <= 0)
        {
            m_fSpeedModifier = 1.0f;
            m_pModel->SetAnimationSpeed(m_fAnimationSpeed);
            m_bSlowed = false;
        }
    }
    
    // Check for collisions with villagers and pick them up if possible
    if (m_eState == WalkState && m_iCurrentVillagersCarried < m_iVillagersCarried)
    {
        if (VillagerManager::CollisionCheck(m_pModel->GetLocation()))
        {
            Villager* villager = VillagerManager::GetCollidedVillager(m_pModel->GetLocation());
            villager->PickUp();
            PlayCaptureSound();
            m_iCurrentVillagersCarried++;
            
            // If they have their maximum amount of villagers, turn around and leave
            if(!m_bIsLeaving && m_iCurrentVillagersCarried == m_iVillagersCarried)
            {
                m_bIsLeaving = true;
                SetShortestPath(m_lOriginalPath);
                m_lExitPath.pop_front();
                m_pModel->Rotate(GetAngleToNextTile());
            }
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
            m_fDeathTimeLeft = 2.5f;
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

        // Drop any carried villagers
        while(m_iCurrentVillagersCarried > 0)
        {
            VillagerManager::Add(m_pModel->GetLocation(), m_tTargetTile, m_lOriginalPath);
            m_iCurrentVillagersCarried--;
        }

        if (m_eKillingAttack == AT_FIRE)
        {
            m_fDeathTimeLeft -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

            // Stop the particle effect when the death timer is 0
            if (m_fDeathTimeLeft <= 0.0f)
                m_pParticleSystem->setEmitting(false);
            // And start it when there are 2 seconds left
            else if (m_fDeathTimeLeft <= 1.5f)
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
    else if (m_eState == WalkState && !m_bCanDelete)  // Continue walking along the path
    {

        //NICK~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
        //If the enemy is going to the village
        if(!m_bIsLeaving)
        {
            //We find the angle between the enemy and the next tile he wants to go to
            float angle = GetAngleToNextTile();

            //We then find what the difference is between the model's current orientation and the angle to the next tile. We also
            // multiply it by the timeSinceLastFrame so that the enemy doesn't turn instantly. We also multiply it by an arbitrary
            // number so that the enemy rotates fast enough.
            float rotation = GetAngleDifference(m_pModel->GetRotation(), angle) * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() * 2.5 * m_fWalkSpeed * m_fSpeedModifier;
            
            //If the rotation isn't redundant
            if(rotation > 0.001 || rotation < -0.001)
            {
                //Rotate the model
                m_pModel->Rotate(rotation);
            }

            //We then move the model forward.
            m_pModel->MoveForward(m_fWalkSpeed * m_fSpeedModifier * Tile::GetDefaultSize() * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed());

            //If the enemy has reached the village
            if(m_lCurrentPath.size() == 0)
            {
                //We flip the bool allowing them to leave
                m_bIsLeaving = true;
                
                //We remove the desired amount of villagers from the village
                m_iCurrentVillagersCarried = VillagerManager::RemoveFromVillage(m_iVillagersCarried);
                PlayCaptureSound();

                //And we flip the model around instantly so that they don't fly off the path while trying to turn around.
                m_pModel->Rotate(GetAngleToNextTile());
            }
            //If they haven't reached the village
            else
            {
               //If the enemy has reached the target tile
               if(Tile(m_pModel->GetLocation()) == m_tTargetTile)
               {
                   //Pop it off the list
                   m_lCurrentPath.pop_front();

                   //NICK~*~*~*~*~*~*~*~*~*~*~*
                   if(!m_bIsOnCorrectPath)
                   {
                       SetShortestPath(m_lReceivedPath);
                   }
                   //~*~*~*~*~*~*~*~*~*~*~*~*~*
               }
               
               //If the list isn't empty
               if(m_lCurrentPath.size() != 0)
               {
                   //Get the next tile
                   m_tTargetTile = m_lCurrentPath.front();
               }
            }
        }
        //If the enemy reached the village and is leaving
        else
        {
            //Movement is the same as above
            float angle = GetAngleToNextTile();

            float rotation = GetAngleDifference(m_pModel->GetRotation(), angle) * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() * 2.5 * m_fWalkSpeed * m_fSpeedModifier;
            
            if(rotation > 0.001 || rotation < -0.001)
            {
                m_pModel->Rotate(rotation);
            }
            
            m_pModel->MoveForward(m_fWalkSpeed * m_fSpeedModifier * Tile::GetDefaultSize() * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed());

            //If the enemy reaches the end of the path
            if(m_lExitPath.size() == 0)
            {
                VillagerManager::VillagersStolen(m_iCurrentVillagersCarried);
                PlayEscapeSound();

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
        //~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
    }
    else if(m_eState == EscapeState)
    {
        m_fTimeSinceEscape += p_feEvent.timeSinceLastFrame * m_fWalkSpeed * m_fSpeedModifier * BaseApplication::GetGameSpeed();
        Ogre::Camera* tempCam = Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->getCamera("PlayerCam");

        if(tempCam->isVisible(m_pModel->GetEntity()->getBoundingBox()) || m_fTimeSinceEscape > 8.0f)
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

void BaseEnemy::Select()
{
    SelectionCircle::LockTo(m_pModel->GetSceneNode(), m_fCircleSize, m_fCircleOffset);
}

/**************************************************************************************
*	Show the bounding box of the entity. (Added by Pierre)
***************************************************************************************/
void BaseEnemy::ShowObject(bool p_bIsShow)
{
    // If it not show the bounding box but it want to show the box, show the box
	if(p_bIsShow && m_pModel != 0 && !m_bIsShow)
	{
		//m_pModel->GetSceneNode()->showBoundingBox(true);
        //m_pBoundingBox->setVisible(true);
        SelectionCircle::HoverOver(m_pModel->GetSceneNode(), m_fCircleSize, m_fCircleOffset);
		m_bIsShow = true;
	}

	// If it shows the bounding box but it don't want to show the box, disable it
	else if (!p_bIsShow && m_pModel != 0 && m_bIsShow)
	{
		//m_pModel->GetSceneNode()->showBoundingBox(false);
        //m_pBoundingBox->setVisible(false);
        SelectionCircle::HideHoverCircle();
		m_bIsShow = false;
	}
}

//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
//NICK: Finds the smallest difference between two angles
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
float BaseEnemy::GetAngleDifference(float p_fCurrent, float p_fDestination)
{
    //if the absolute value of the difference between the desired angle and the current angle is less then 180
    if(p_fDestination - p_fCurrent < 180 && p_fDestination - p_fCurrent > -180)
    {
        //return that difference
        return p_fDestination - p_fCurrent;
    }
    //If the difference is greater then 180 and the destination angle is higher then 180
    else if (p_fDestination > 180)
    {
        //It means the difference between the destination and the current angle is greater then 180, so we
        // subtract 360 degrees off it to bring it below 180.
        return p_fDestination - 360 - p_fCurrent;
    }
    else
    {
        //Otherwise, the difference between the two must be less then -180, so we add 360 degrees to it.
        return p_fDestination - p_fCurrent + 360;
    }
}

/**************************************************************************************
*	Handle the events. (Added by Pierre)
***************************************************************************************/
void BaseEnemy::HandleEvent(const Hills::GameEvent& p_geGameEvent)
{
	// If it is the open new path event
	if(p_geGameEvent.GetGameEventType() == Hills::GE_OpenNewPath)
	{
		SetShortestPath(p_geGameEvent.GetShortestPath());
	}
    // Healing event
    else if (p_geGameEvent.GetGameEventType() == Hills::GE_Heal)
    {
		Ogre::Vector3 loc = m_pModel->GetLocation();
		float inRange = sqrt(pow(loc.x - p_geGameEvent.GetOrigin().x, 2) + 
                             pow(loc.z - p_geGameEvent.GetOrigin().z, 2));
		
        if (inRange <= Tile::GetDefaultSize() * 1.5  &&
            m_pModel->GetSceneNode()->getName() != p_geGameEvent.GetID())  // Arbitrarily defined range
        {
            Healed(p_geGameEvent.GetAmount());
            m_pHealParticle->setEmitting(true);
            m_fHealEmitter = 0.2f;
            m_bHealed = true;
        }
    }
    // Area attack event
    else if (p_geGameEvent.GetGameEventType() == Hills::GE_AreaAttack)
    {
        // Circular area attack
        if (p_geGameEvent.GetAreaOfEffectType() == Hills::AOE_Sphere)
        {
		    Ogre::Vector3 loc = m_pModel->GetLocation();
		    float inRange = sqrt(pow(loc.x - p_geGameEvent.GetOrigin().x, 2) + 
                                 pow(loc.z - p_geGameEvent.GetOrigin().z, 2));

            if (inRange <= p_geGameEvent.GetRadius())
                Attacked(p_geGameEvent.GetAmount(), false, AT_STANDARD);
        }
        // Conal area attack
        else if (p_geGameEvent.GetAreaOfEffectType() == Hills::AOE_Cone)
        {
		    // First check if this enemy is within range of the attack
            Ogre::Vector3 loc = m_pModel->GetLocation();
		    float inRange = sqrt(pow(loc.x - p_geGameEvent.GetOrigin().x, 2) + 
                                 pow(loc.z - p_geGameEvent.GetOrigin().z, 2));

            if (inRange <= p_geGameEvent.GetRadius())
            {
                // Set the maximum angle for our cone, making sure it is within 0-360
                int max = (p_geGameEvent.GetDirection() + 90) + p_geGameEvent.GetAngle()/2;
                if (max <= 0)
                    max += 360;
                else if (max >= 360)
                    max -= 360;
                // Do the same for the minimum angle
                int min = (p_geGameEvent.GetDirection() + 90) - p_geGameEvent.GetAngle()/2;
                if (min <= 0)
                    min += 360;
                else if (min >= 360)
                    min -= 360;

                // Calculate the angle that the enemy is at
                float deltaX = m_pModel->GetLocation().x - p_geGameEvent.GetOrigin().x;
                float deltaZ = m_pModel->GetLocation().z - p_geGameEvent.GetOrigin().z;

                int angle = atan2(deltaZ, deltaX) * 180 / 3.14159;
                if (angle <= 0)
                    angle += 360;
                else if (angle >= 360)
                    angle -= 360;

                // Have to be careful here, as our max may spill over past 360 and end up
                // being a lower number than our minimum, so we check for that first
                if (max < min)
                {
                    if (angle <= max && angle+360 >= min)
                        Attacked(p_geGameEvent.GetAmount(), false, AT_FIRE);
                    else if (angle-360 <= max && angle >= min)
                        Attacked(p_geGameEvent.GetAmount(), false, AT_FIRE);
                }
                // If the max is larger than the minimum, we do a standard check
                else if (angle <= max && angle >= min)
                    Attacked(p_geGameEvent.GetAmount(), false, AT_FIRE);
            }
        }
    }
    // Movement speed reduction event
    else if (p_geGameEvent.GetGameEventType() == Hills::GE_SlowAttack)
    {
        Ogre::Vector3 loc = m_pModel->GetLocation();
		float inRange = sqrt(pow(loc.x - p_geGameEvent.GetOrigin().x, 2) +
                             pow(loc.z - p_geGameEvent.GetOrigin().z, 2));
		
        // Only slow if the target is in range and this slow is stronger than the current
        if (inRange <= p_geGameEvent.GetRadius() && m_fSpeedModifier >= p_geGameEvent.GetAmount())
        {
            m_bSlowed = true;
            m_fSlowTime = 3.0f;
            m_fSpeedModifier = p_geGameEvent.GetAmount();
            m_pModel->SetAnimationSpeed(m_fAnimationSpeed * p_geGameEvent.GetAmount());
        }
    }
    else if (p_geGameEvent.GetGameEventType() == Hills::GE_DayNightChange)
    {
        m_bIdentified = false;
    }
}

//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
//NICK: Find the angle to the next tile
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
float BaseEnemy::GetAngleToNextTile()
{
    //We adjust the destination tile by the enemies offset. We also assign Y a value of 0
    // because it's irrelevant.
    Ogre::Vector3 destination = Ogre::Vector3(m_tTargetTile.GetLocation().x + m_fXOffset, 0,
        m_tTargetTile.GetLocation().z + m_fZOffset);

    Ogre::Vector3 direction = destination - m_pModel->GetLocation();

    float angle;

    //We have different cases because finding the atan will yeild a number that doesn't match
    //up correctly with each of the quadrents.

    //      270
    //    -x | +x
    //    -z | -z
    // 180 ------- 0
    //    -x | +x
    //    +z | +z
    //       90
    if(direction.z < 0)
    {
        //If the Z direction is negative, we add 180 degrees to our angle regardless of the X
        float radians = atan(direction.x / direction.z);
        angle = radians / 2 / 3.1415926535 * 360 + 180;
    }
    else if(direction.x < 0)
    {
        //If Z is positive and X is negative, we add 360
        float radians = atan(direction.x / direction.z);
        angle = 360 + radians / 2 / 3.1415926535 * 360;
    }
    else if(direction.x > 0)
    {
        //If they're both positive, then we just hand back the calculated angle
        float radians = atan(direction.x / direction.z);
        angle = radians / 2 / 3.1415926535 * 360;
    }
    else
    {
        //If they're both 0 then you're literally standing on the destination.
        angle = m_pModel->GetRotation();
    }

    return angle;
}

//-----------------------------------------------------------------------------
// Special animation to perform if an enemy is killed by a fire attack
//-----------------------------------------------------------------------------
void BaseEnemy::DieByFire()
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

void BaseEnemy::PlayDeathSound()
{
    int preSelection = rand() % 221;

    if(preSelection == 220)
    {
        SoundManager::Play3DSound("WilhelmScream.wav", m_pModel->GetLocation());
    }
    else
    {
        int selection = preSelection % 22;

        std::string fileNames[22];

        fileNames[0] = "enemyDeath01.wav";
        fileNames[1] = "enemyDeath02.wav";
        fileNames[2] = "enemyDeath03.wav";
        fileNames[3] = "enemyDeath04.wav";
        fileNames[4] = "enemyDeath05.wav";
        fileNames[5] = "enemyDeath06.wav";
        fileNames[6] = "enemyDeath07.wav";
        fileNames[7] = "enemyDeath08.wav";
        fileNames[8] = "enemyDeath09.wav";
        fileNames[9] = "enemyDeath10.wav";
        fileNames[10] = "enemyDeath11.wav";
        fileNames[11] = "enemyDeath12.wav";
        fileNames[12] = "enemyDeath13.wav";
        fileNames[13] = "enemyDeath14.wav";
        fileNames[14] = "enemyDeath15.wav";
        fileNames[15] = "enemyDeath16.wav";
        fileNames[16] = "enemyDeath17.wav";
        fileNames[17] = "enemyDeath18.wav";
        fileNames[18] = "enemyDeath19.wav";
        fileNames[19] = "enemyDeath20.wav";
        fileNames[20] = "enemyDeath21.wav";
        fileNames[21] = "enemyDeath22.wav";

        SoundManager::Play3DPitchedSound(fileNames[selection], m_pModel->GetLocation(), 0.15f, 1.0f, m_fScreamPitch);
    }
}

void BaseEnemy::PlayCaptureSound()
{
    int selection = rand() % 9;

    std::string fileNames[9];

    fileNames[0] = "villagerTaken1.wav";
    fileNames[1] = "villagerTaken2.wav";
    fileNames[2] = "villagerTaken3.wav";
    fileNames[3] = "villagerTaken4.wav";
    fileNames[4] = "villagerTaken5.wav";
    fileNames[5] = "villagerTaken6.wav";
    fileNames[6] = "villagerTaken7.wav";
    fileNames[7] = "villagerTaken8.wav";
    fileNames[8] = "villagerTaken9.wav";

    SoundManager::Play3DPitchedSound(fileNames[selection], m_pModel->GetLocation(), 0.15);
}

void BaseEnemy::DieBySniper()
{

}