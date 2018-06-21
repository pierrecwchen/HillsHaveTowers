//-----------------------------------------------------------------------------
// Player.cpp                                               By: Keenan Johnston
// 
// Implementation file for BaseTower.h
//-----------------------------------------------------------------------------

#include "Player.h"
#include "GameDataManager.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Player::Player(std::string p_sMesh)
{
    // Default player stats
    m_fMoveSpeed = 2.0f;
    m_fBuildSpeed = 1.0f;
    m_fHarvestSpeed = m_fTimeUntilNextHarvest = 2.0f;
    m_fSpeedMultiplier = 1.0f;
    m_iHeight = 0;

    // Set flags to false by default
    //m_bToBuild = false;
    //m_bToHarvest = false;
    //m_bToUpgrade = false;

    m_bMadeHarvestNoise = true;

    m_iCurrentPower = -1;
    
    // Create the visual representation of the player and play its idle animation
    m_pModel = new Model(p_sMesh);
    m_pModel->SetAnimationBlending(true);
    m_pModel->Translate(Ogre::Vector3(250, Terrain::GetHeight(250, 250) + m_iHeight, 250));
    m_pModel->Scale(Ogre::Vector3(0.2));
    m_pModel->SetAnimationSpeed(0.5);
    m_pModel->UseTerrainHeight(false);
    m_pModel->GetSceneNode()->yaw(Ogre::Degree(90));
    m_pModel->PlayAnimation("Idle4", true);

    m_tCurrentTile = Tile(m_pModel->GetLocation());

    m_fStunTime = 4.0f;  // default stun time when hit by enemy
    
    // Default to idle state
    m_eState = PlayerIdleState;

    m_eTower = TTB_ArrowTower;
	
	/***********************************************************************************
	*	Added by Pierre
	************************************************************************************/
	//m_tSelectableObj = SO_Tile;
	m_tRecipient = Hills::R_Player;
	//SelectableManager::Add(this);
	Hills::EventManager::AddRecipient(this, 1, Hills::GE_NULL);
	/***********************************************************************************/
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
Player::~Player()
{
	Hills::EventManager::RemoveRecipient(this);

	m_lCurrentPath.clear();

	m_pDestination = 0;
	m_pTowerToUpgrade = 0;
	m_pResourceToHarvest = 0;

	m_pModel->CleanUp();
    delete m_pModel;
}

//-----------------------------------------------------------------------------
// Pass a pointer to the resource to be harvested, set the harvest interval and
// put the player into the harvesting state
//-----------------------------------------------------------------------------
void Player::Harvest(BaseResource* p_pResource)
{
    m_fTimeUntilNextHarvest = m_fHarvestSpeed;
    m_pResourceToHarvest = p_pResource;
    m_eState = PlayerHarvestState;
}

//-----------------------------------------------------------------------------
// Build a new tower on the given tile if resources allow
//-----------------------------------------------------------------------------
void Player::BuildTower(Towers p_eType, Tile* p_tTile)
{
    // Need to get coordinates of where tower is to be placed
    //TowerManager::Instance()->Add(p_eType, p_tTile->GetLocation());
}

//-----------------------------------------------------------------------------
// Upgrade an existing tower if resources allow
//-----------------------------------------------------------------------------
void Player::UpgradeTower(BaseTower* p_pTower)
{
    // Need to check if player has enough resources
    //p_pTower->Upgrade();
}

//-----------------------------------------------------------------------------
// Set up movement to the given location
//-----------------------------------------------------------------------------
void Player::MoveTo(Tile* p_pTile)
{
    // Reset harvest speed
    m_fTimeUntilNextHarvest = 2.0f;

    // If the player is not currently knocked down, move to the given tile
    if (m_eState != PlayerKnockdownState)
    {
        // We put the player to walk state regardless of whether he is in the selected tile already
        // or not so we can determine if he needs to build a tower there, which happens in the walk state
        m_eState = PlayerWalkState;
        
        //NICK~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
        //TempTile: The tile we're currently standing on
        Tile tempTile(m_pModel->GetLocation());

        //If the tile we're going to is not the tile we're already standing on
        if(tempTile != *p_pTile)
        {
            //We set m_pDestination to hold the final tile
            m_pDestination = p_pTile;

            //We find the shortest player path from our current location to the target tile
            m_lCurrentPath = TileSet::GetShortestPathFromTo(Tile(m_pModel->GetLocation()), *p_pTile, 1);

            if(m_lCurrentPath.empty())
            {
                m_eState = PlayerRejectState;
                m_v3FinalLocation = m_pModel->GetLocation();
                return;
            }

            //If the last tile in the path is a resource
            if(!TileSet::GetTileAt(m_lCurrentPath.back())->IsWalkableTerrain())
            {
                //Get rid of it, we don't want the player to walk into it
                m_lCurrentPath.pop_back();
            }

            //If the path isn't just 1 tile long
            if(m_lCurrentPath.size() != 1)
            {
                //We set our final world position as the back of the path
                m_v3FinalLocation = m_lCurrentPath.back();

                //And we find out which tile we're headed towards
                FindTargetTile();
            }
            else
            {
                //If the path is only 1 tile long, then we just set that tile as our destination and target
                m_v3FinalLocation = m_lCurrentPath.back();
                m_v3TargetLocation = m_lCurrentPath.back();
                m_lCurrentPath.pop_back();
            }
        }
        //~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
    }
}

void Player::PreparePower(int p_iIndex)
{
    if(m_vPowers[p_iIndex]->IsReady())
    {
        if(m_vPowers[p_iIndex]->TypeRequired() != SO_NULL)
        {
            m_iCurrentPower = p_iIndex;
        }
        else
        {
            m_vPowers[p_iIndex]->Activate(0);
        }
    }
    else
    {
        PlayRejectSound();
    }
}

void Player::ActivatePower(Selectable* p_sTarget)
{
    if(p_sTarget->GetSelectableObjType() == m_vPowers[m_iCurrentPower]->TypeRequired())
    {
        m_vPowers[m_iCurrentPower]->Activate(p_sTarget);
        m_iCurrentPower = -1;
    }
    else
    {
        m_eState = PlayerRejectState;
    }
}

//-----------------------------------------------------------------------------
// Determine the player's state and update them accordingly
//-----------------------------------------------------------------------------
void Player::Update(const Ogre::FrameEvent& p_feEvent)
{
    float frameLength = p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() * m_fSpeedMultiplier;

    for(int j = 0; j < m_vPowers.size(); j++)
    {
        m_vPowers[j]->Update(p_feEvent);
    }
    // Decrease the immunity time of the player if they are recovering from a knockdown
    // and blink the character to indicate this
    if (m_fImmunityTime > 0)
    {
        m_fImmunityTime -= frameLength;
        m_fBlinkTime  -= frameLength;

        if (m_fBlinkTime <= 0)
        {
            m_pModel->GetSceneNode()->flipVisibility();
            m_fBlinkTime = 0.2f;
        }
    }
    else if (!m_pModel->GetEntity()->isVisible())
        m_pModel->GetEntity()->setVisible(true);
    
    // Check for collisions with enemies
    if (EnemyManager::Instance()->EnemiesInRange(m_pModel->GetLocation(), Tile::GetDefaultSize()/1.75) && 
        m_eState != PlayerKnockdownState && m_fImmunityTime <= 0)
    {
        // Get the vector of enemies within the collision zone (hopefully only 1)
        std::vector<BaseEnemy*> enemies = EnemyManager::Instance()->GetEnemiesInRange(m_pModel->GetLocation(), Tile::GetDefaultSize()/1.75);
        Ogre::Vector3 direction = (enemies.at(0)->GetLocation() - m_pModel->GetLocation())*3;
        Ogre::Vector3 position = m_pModel->GetLocation() - direction;

        m_pModel->StopMovement();
        m_pModel->UseTerrainHeight(true);
        
        if(TileSet::GetTileAt(m_pModel->GetLocation())->IsHorizontalBridge())
        {
            if(direction.x >= 0)
            {
                m_pModel->TranslateToOverTime(0.5, Ogre::Vector3(position.x,
                                                   m_pModel->GetLocation().y,
                                                   m_pModel->GetLocation().z));
            }
            else
            {
                m_pModel->TranslateToOverTime(0.5, Ogre::Vector3(-position.x,
                                                   m_pModel->GetLocation().y,
                                                   m_pModel->GetLocation().z));
            }
        }
        else if(TileSet::GetTileAt(m_pModel->GetLocation())->IsVerticalBridge())
        {
            if(direction.z >= 0)
            {
                m_pModel->TranslateToOverTime(0.5, Ogre::Vector3(m_pModel->GetLocation().x,
                                                                 m_pModel->GetLocation().y,
                                                                 position.z));
            }
            else
            {
                m_pModel->TranslateToOverTime(0.5, Ogre::Vector3(m_pModel->GetLocation().x,
                                                                 m_pModel->GetLocation().y,
                                                                 -position.z));
            }
        }
        else
        {
            m_pModel->TranslateToOverTime(0.5, Ogre::Vector3(position.x, m_pModel->GetLocation().y, position.z));
        }
        m_eState = PlayerKnockdownState;
        if (m_eAnimState != AS_SPIN)
        {
            m_pModel->SetAnimationBlending(false); // Don't blend into this animation
            m_pModel->PlayAnimation("Spin", true);
            m_pModel->SetAnimationBlending(true);
            m_eAnimState = AS_SPIN;
        }
    }
    
    //-------------------------------------------------
    // Player Idle State
    //-------------------------------------------------
    if (m_eState == PlayerIdleState) // Player standing still
    {
        // Make sure idle animation is playing
        if (m_eAnimState != AS_IDLE)
        {
            m_pModel->PlayAnimation("Idle4", true);
            m_eAnimState = AS_IDLE;
        }
    }
    else if (m_eState == PlayerRejectState)
    {
        if (m_eAnimState != AS_NO)
        {
            m_pModel->PlayAnimation("Negative", false);
            PlayRejectSound();
            m_eAnimState = AS_NO;
        }

        if (m_pModel->HasAnimationEnded())
        {
            m_eState = PlayerIdleState;
        }
    }
    //-------------------------------------------------
    // Player Walking State
    //-------------------------------------------------
    else if (m_eState == PlayerWalkState)
    {
        // Make sure walk animation is playing
        if (m_eAnimState != AS_WALK)
        {
            m_pModel->PlayAnimation("Walk", true);
            m_eAnimState = AS_WALK;
        }
        
        //*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
        //Added by Nick
        //*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
        if (m_pModel->GetLocation().distance(m_v3FinalLocation) > (Tile::GetDefaultSize() / 4))
        {
            //We find the angle to the player's next destination
            float angle = GetAngleToNextTile();

            //We find the shortest rotation to it and then multiply it by the timeSinceLastFrame so that he turns slowly.
            // It's then multiplied by 10 because otherwise the player starts taking farmer's turns into resources.
            float rotation = GetAngleDifference(m_pModel->GetRotation(), angle) * frameLength * 10;

            //If the rotation isn't redundent, we rotate the player.
            if(rotation > 0.001 || rotation < -0.001)
            {
                m_pModel->Rotate(rotation);
            }

            //We then push the player a little bit forward.
            m_pModel->MoveForwardOverTerrain(m_fMoveSpeed * Tile::GetDefaultSize() * BaseApplication::GetGameSpeed() * m_fSpeedMultiplier,
                                             p_feEvent.timeSinceLastFrame, (float)m_iHeight);          

            //If the player's next target isn't the final tile
            if(m_v3TargetLocation != m_v3FinalLocation)
            {
                //We perform a check to find out if we can still walk to that tile.
                FindTargetTile();
            }
        }
        //*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
        else if (m_eAction == PA_BUILD)
        {
            if (m_eTower == TTB_ArrowTower)
            {
                MainCurrency::Subtract(ArrowTower::GetCost(0));
                TowerManager::Instance()->Add(eArrowTower, m_pDestination);
            }
            else if (m_eTower == TTB_CannonTower)
            {
                MainCurrency::Subtract(CannonTower::GetCost(0));
                TowerManager::Instance()->Add(eCannonTower, m_pDestination);
            }
            else if (m_eTower == TTB_BurstTower)
            {
                MainCurrency::Subtract(BurstTower::GetCost(0));
                TowerManager::Instance()->Add(eBurstTower, m_pDestination);
            }
            else if (m_eTower == TTB_SniperTower)  
            {
                MainCurrency::Subtract(SniperTower::GetCost(0));
                TowerManager::Instance()->Add(eSniperTower, m_pDestination);
            }
            else if (m_eTower == TTB_SlowTower) 
            {
                MainCurrency::Subtract(SlowTower::GetCost(0));
                TowerManager::Instance()->Add(eSlowTower, m_pDestination);
            }
            m_eState = PlayerIdleState;
        }
        else if (m_eAction == PA_HARVEST)
        {
            m_eState = PlayerHarvestState;
            m_pModel->FaceTo(m_pResourceToHarvest->GetLocation());
        }
        else if (m_eAction == PA_UPGRADE)
        {
            if (m_pTowerToUpgrade->CanInteract())
            {
                MainCurrency::Subtract(m_pTowerToUpgrade->GetUpgradeCost(m_pTowerToUpgrade->GetLevel()));
                m_pTowerToUpgrade->Upgrade();
            }
            m_eState = PlayerIdleState;
        }
        else if (m_eAction == PA_REPAIR)
        {
            m_pTowerToRepair = TowerManager::Instance()->GetSelectedTower();
            m_eState = PS_REPAIRING;
            m_fRepairTime = 1.0f;
        }
        else
            m_eState = PlayerIdleState;
    }
    //-------------------------------------------------
    // Player Harvesting State
    //-------------------------------------------------
    else if (m_eState == PlayerHarvestState)
    {
        // Play a harvesting animation (or idle if we have none)
        if (m_pResourceToHarvest->GetType() == RT_Wood)
        {
            if (m_eAnimState != AS_CHOP)
            {
                m_pModel->PlayAnimation("Attack1", true);
                m_eAnimState = AS_CHOP;
            }
        }
        else if (m_pResourceToHarvest->GetType() == RT_Stone || 
                 m_pResourceToHarvest->GetType() == RT_Iron)
        {
            if (m_eAnimState != AS_MINE)
            {
                m_pModel->PlayAnimation("Attack2", true);
                m_eAnimState = AS_MINE;
            }
        }

        if (m_pResourceToHarvest->GetType() == RT_Wood)
        {
            if(m_pModel->GetAnimationState()->getTimePosition() > 0.1867 && !m_bMadeHarvestNoise)
            {
                PlayHarvestSound();
                m_bMadeHarvestNoise = true;
            }
            else if(m_pModel->GetAnimationState()->getTimePosition() < 0.1867 && m_bMadeHarvestNoise)
            {
                m_bMadeHarvestNoise = false;
            }
        }
        else
        {
            if(m_pModel->GetAnimationState()->getTimePosition() > 0.2987 && !m_bMadeHarvestNoise)
            {
                PlayHarvestSound();
                m_bMadeHarvestNoise = true;
            }
            else if(m_pModel->GetAnimationState()->getTimePosition() < 0.2987 && m_bMadeHarvestNoise)
            {
                m_bMadeHarvestNoise = false;
            }
        }

        m_fTimeUntilNextHarvest -= frameLength;

        if (m_fTimeUntilNextHarvest <= 0) // if enough time has passed, harvest the resource
        {
            m_pResourceToHarvest->Harvest();

            if (m_pResourceToHarvest->IsHarvestable()) // if it still has more, reset the interval time
                m_fTimeUntilNextHarvest = m_fHarvestSpeed;
            else // if it has been depleted, go back to the idle state
            {
                m_eState = PlayerIdleState;
                m_bMadeHarvestNoise = true;
            }
        }
    }

    else if (m_eState == PS_REPAIRING)
    {
        m_fRepairTime -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

        // Play an animation for repairing
        if (m_eAnimState != AS_CHOP)
        {
            m_pModel->PlayAnimation("Attack1", true);
            m_eAnimState = AS_CHOP;
        }

        if (m_fRepairTime <= 0)
        {
            m_pTowerToUpgrade->Unfreeze();
            m_pTowerToUpgrade = 0;
            m_eState = PlayerIdleState;
        }
    }

    //-------------------------------------------------
    // Player Knocked Down State
    //-------------------------------------------------
    else if (m_eState == PlayerKnockdownState)  // Player stunned by enemy
    {
        m_fStunTime -= frameLength;

        if (!m_pModel->IsTranslating() && m_eAnimState != AS_DEATH)  // knockback is finished
        {
            m_pModel->PlayAnimation("Die1", false);
            m_pModel->UseTerrainHeight(false);
            m_eAnimState = AS_DEATH;
        }

        if (m_fStunTime <= 0)  // stun finished
        {
            m_eState = PlayerIdleState;
            m_fStunTime = 4.0f;
            m_fImmunityTime = 2.0f;
            m_fBlinkTime = 0.2f;
        }
    }

    m_pModel->Update(p_feEvent); // Update the player's model each frame

	if(GameDataManager::GetSingletonPtr())
		GameDataManager::GetSingletonPtr()->GetCurrentGameData()->SetPlayerLoc(GetLocation()); // Update player location in game data(by Pierre)
}

void Player::PlayHarvestSound()
{
    if(m_pResourceToHarvest->GetType() == RT_Wood)
    {
        int selection = rand() % 10;

        std::string fileNames[10];

        fileNames[0] = "woodChop0.wav";
        fileNames[1] = "woodChop1.wav";
        fileNames[2] = "woodChop2.wav";
        fileNames[3] = "woodChop3.wav";
        fileNames[4] = "woodChop4.wav";
        fileNames[5] = "woodChop5.wav";
        fileNames[6] = "woodChop6.wav";
        fileNames[7] = "woodChop7.wav";
        fileNames[8] = "woodChop8.wav";
        fileNames[9] = "woodChop9.wav";

        SoundManager::Play3DSound(fileNames[selection], m_pResourceToHarvest->GetLocation());
    }
    else
    {
        int selection = rand() % 7;

        std::string fileNames[10];

        fileNames[0] = "pickaxe0.wav";
        fileNames[1] = "pickaxe1.wav";
        fileNames[2] = "pickaxe2.wav";
        fileNames[3] = "pickaxe3.wav";
        fileNames[4] = "pickaxe4.wav";
        fileNames[5] = "pickaxe5.wav";
        fileNames[6] = "pickaxe6.wav";

        SoundManager::Play3DSound(fileNames[selection], m_pResourceToHarvest->GetLocation());
    }
}

void Player::PlayRejectSound()
{
    int selection = rand() % 3;

    std::string fileNames[3];

    fileNames[0] = "playerNo1.wav";
    fileNames[1] = "playerNo2.wav";
    fileNames[2] = "playerNo3.wav";

    SoundManager::Play3DSound(fileNames[selection], m_pModel->GetLocation(), 0.75f);
}

//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
//NICK: Finds out if there's anything obstructing one position and another.
bool Player::CanWalkFromTo(Ogre::Vector3 p_v3Start, Ogre::Vector3 p_v3Destination)
{
    //We find the direction the player needs to walk in and normalize it
    Ogre::Vector3 direction = p_v3Destination - p_v3Start;
    direction.normalise();

    //We find the distance between the player and his destination
    Ogre::Real distance = p_v3Start.distance(p_v3Destination);

    //The origin of the ray is set to be the start moved up two units in the Y. This is so that the ray doesn't
    // originate inside of the tile and instead originates over top of it.
    Ogre::Vector3 position = p_v3Start;
    position.y += 5;
    Ogre::Ray rayToTile(position, direction);
    
    if(TileSet::HasWater())
    {
        vector<Ogre::AxisAlignedBox> waterList = TileSet::GetWaterBoundingBoxes();

        for(int p = 0; p < waterList.size(); p++)
        {
            //We check if the ray intersects the tile's bounding box
            std::pair<bool, Ogre::Real> rayResults = rayToTile.intersects(waterList[p]);

            if(rayResults.first)
            {
                if(rayResults.second < distance)
                {
                    return false;
                }
            }
        }
    }

    //For every tile
    for(int k = 0; k < TileSet::GetLength(); k++)
    {
        for(int j = 0; j < TileSet::GetWidth(); j++)
        {
            //We check if the ray intersects the tile's bounding box
            std::pair<bool, Ogre::Real> rayResults = rayToTile.intersects(TileSet::GetTileAt(j, k)->GetBoundingBox());

            //If there is an intersection
            if(rayResults.first)
            {
                //If it's actually closer to us then our target tile
                if(rayResults.second < distance && *TileSet::GetTileAt(j, k) != p_v3Start && *TileSet::GetTileAt(j, k) != p_v3Destination)
                {
                    //If it intersects with one of the polygons, we can't go straight to the tile so we
                    // return false
                    if(Terrain::IntersectsWithTile(rayToTile, j, k, Tile::GetDefaultSize(),
                        Tile::GetXOffset(), Tile::GetZOffset()).second < distance) return false;
                }
            }

        }
    }

    //If none of the tiles are intersected, we do a RaySceneQuery to check the resources
    Ogre::RaySceneQuery* raySQ = Ogre::Root::getSingletonPtr()->
                                getSceneManager(BaseApplication::GetSceneManagerName())->
                                createRayQuery(rayToTile);
    raySQ->setSortByDistance(true);
    
    position.y -= 5;

    //We set the query mask so that it only checks the resources and then execute our check
    raySQ->setQueryMask(QF_RESOURCES | QF_TERRAIN);
    Ogre::RaySceneQueryResult queryResults = raySQ->execute();
    Ogre::RaySceneQueryResult::iterator it = queryResults.begin();

    //For all the intersections the ray caught
    for(it; it != queryResults.end(); it++)
    {
        //If it's closer to us then our destination, we can't go there
        if(it->distance < distance && it->distance > 0) return false;
    }
    
    //We move the ray up a little bit higher, because the lower ray cast seems to miss resources that are on top of hills.
    position.y += 3;

    Ogre::Ray higherRay(position, direction);

    raySQ->setRay(higherRay);
    queryResults = raySQ->execute();
    it = queryResults.begin();

    for(it; it != queryResults.end(); it++)
    {
        if(it->distance < distance && it->distance > 0) return false;
    }

    //If not a single intersection occured before our destination, we can go there
    return true;
}

//~*~*~*~*~*~*~*~*~*
//NICK: Refer to BaseEnemy::GetAngleDifference
//~*~*~*~*~*~*~*~*~*
float Player::GetAngleDifference(float p_fCurrent, float p_fDestination)
{
    if(p_fDestination - p_fCurrent < 180 && p_fDestination - p_fCurrent > -180)
    {
        return p_fDestination - p_fCurrent;
    }
    else if (p_fDestination > 180)
    {
        return p_fDestination - 360 - p_fCurrent;
    }
    else
    {
        return p_fDestination - p_fCurrent + 360;
    }
}

/**************************************************************************************
*	Handle the events. (Added by Pierre)
***************************************************************************************/
void Player::HandleEvent(const Hills::GameEvent& p_geGameEvent)
{
    
}

//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
//NICK: Finds the farthest away tile that we can walk to.
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
void Player::FindTargetTile()
{
    //We start at the very last tile and work our way down.
    list<Ogre::Vector3>::reverse_iterator it = m_lCurrentPath.rbegin();

    Ogre::Vector3 targetLocation;

    //We create a boolean that identifies when we find the tile we can walk to
    // because the CanWalkFromTo method is rather heavy
    bool targetTileFound = false;

    //For every tile in our path
    for(int j = 0; j < m_lCurrentPath.size() && !targetTileFound; j++)
    {
        targetLocation = *it;   

        //If we can walk to it
        if(CanWalkFromTo(m_pModel->GetLocation(), targetLocation))
        {
            //We set it as our destination
            m_v3TargetLocation = targetLocation;
            targetTileFound = true;
        }

        if(!targetTileFound)
        {
            it++;
        }
    }
}

//~*~*~*~*~*~*~*~*~*
//NICK: Refer to BaseEnemy::GetAngleToNextTile
//~*~*~*~*~*~*~*~*~*
float Player::GetAngleToNextTile()
{
    Ogre::Vector3 direction = m_v3TargetLocation - m_pModel->GetLocation();

    float angle;

    if(direction.z < 0)
    {
        float radians = atan(direction.x / direction.z);
        angle = radians / 2 / 3.1415926535 * 360 + 180;
    }
    else if(direction.x < 0)
    {
        float radians = atan(direction.x / direction.z);
        angle = 360 + radians / 2 / 3.1415926535 * 360;
    }
    else if(direction.x > 0)
    {
        float radians = atan(direction.x / direction.z);
        angle = radians / 2 / 3.1415926535 * 360;
    }
    else
    {
        angle = m_pModel->GetRotation();
    }

    return angle;
}