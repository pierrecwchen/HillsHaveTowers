//-----------------------------------------------------------------------------
// CannonTower.cpp                                          By: Keenan Johnston
// 
// Implementation file for CannonTower.h
//-----------------------------------------------------------------------------

#include "CannonTower.h"

Currency CannonTower::m_cBuildCost[] = { Currency(150,0,0,0) };

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CannonTower::CannonTower(Tile* p_pTile, bool p_bIsNight)
{
    // Set up tower stats
    m_fRange = 1.5 * Tile::GetDefaultSize();
    m_fAtkRate = 1;
    m_fDamage = 150;
    m_fRadius = Tile::GetDefaultSize() * 0.5;
    m_iLevel = 1;

    if(p_bIsNight)
    {
        m_fRangeModifier = 0.65f;
    }
    else
    {
        m_fRangeModifier = 1.0f;
    }

    m_cCost[0] = Currency(150,0,0,0);
    m_cCost[1] = Currency(225,0,3,0);
    m_cCost[2] = Currency(300,0,5,3);

    m_v3ProjectileOffset = Ogre::Vector3(0,6.5,0);
    m_fProjectileSpeed = 50;

    m_bCanInteract = false;
    m_bCanDelete = false;
    m_pTile = p_pTile;
    m_pTile->SetHasTower(true);

    m_fCircleSize = 5.5f;
    m_fCircleOffset = 0.0f;

    // Create the visual representation of the tower
    m_pModel =  new Model("Cannon1_2.mesh");
    m_pModel->Scale(Ogre::Vector3(2.5));
    m_pModel2 = new Model("Cannon1_1.mesh");
    m_pModel2->Scale(Ogre::Vector3(2.4));

    // Move tower to build location
    m_pModel->Translate(m_pTile->GetLocation());
    m_pModel2->Translate(m_pTile->GetLocation());

    m_pModel->Translate(Ogre::Vector3(0,-15,0));  // fix position of seperate parts of the tower
    m_pModel2->Translate(Ogre::Vector3(0,-8.5,0));

    // Setup custom bounding box
    m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
    m_pBoundingBox->attachObject(BoundingBox::GenerateBoundingBox(m_pModel->GetSceneNode()->getName(), BT_TOWER));
    m_pBoundingBox->setVisible(false);
    m_pBoundingBox->translate(0,-2.0,0);
    m_pBoundingBox->scale(Ogre::Vector3(0.25,0.5,0.25));

    // Set tower state to build
    m_eState = TS_BUILD;
    m_fBuildTimeLeft = 3;
    m_bUpgradeReady = false;
    SetSelectable(false);

    // Create and attach dust particle
    m_pBuildEffect = Ogre::Root::getSingletonPtr()->
                     getSceneManager(BaseApplication::GetSceneManagerName())->
                     createParticleSystem(m_pModel->GetSceneNode()->getName() + "Dust", "Dust");

    m_pDustNode = Ogre::Root::getSingletonPtr()->
                  getSceneManager(BaseApplication::GetSceneManagerName())->
                  getRootSceneNode()->createChildSceneNode();
    m_pDustNode->translate(m_pTile->GetLocation());
    m_pDustNode->attachObject(m_pBuildEffect);

    m_iChannelNumber = SoundManager::Play3DLoopedSound("buildingTools.wav", m_pTile->GetLocation(), 0.75f);

    // Raise the tower out of the ground for a build animation
    m_pModel->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,20,0));
    m_pModel2->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,20,0));
}

//-----------------------------------------------------------------------------
// Old constructor
//-----------------------------------------------------------------------------
CannonTower::CannonTower(float p_fRange, float p_fAtkRate, float p_fDamage, float p_fBuildTime,
                         std::string p_sMesh, Ogre::SceneManager* p_smManager)
{
    // Set up tower stats
    m_fRange = p_fRange;
    m_fAtkRate = p_fAtkRate;
    m_fDamage = p_fDamage;
    m_iLevel = 1;

    m_bCanDelete = false;
  
    // Create the visual representation of the tower
    m_pModel = new Model(p_sMesh);

    // Set tower state to idle/build?
    m_eState = TS_BUILD;
    m_fBuildTimeLeft = p_fBuildTime;
}
//-----------------------------------------------------------------------------
// Upgrade the tower to the next level
//-----------------------------------------------------------------------------
void CannonTower::Upgrade()
{
    m_bCanInteract = false;
    SetSelectable(false);
    SelectionCircle::Deselect(m_pModel->GetSceneNode());
    m_pBuildEffect->setEmitting(true);
    
    // Before building the new tower, we want to sink the previous one into the ground
    // over half of the total upgrade time of the tower
    if (!m_bUpgradeReady)
    {
        m_iChannelNumber = SoundManager::Play3DLoopedSound("towerDescend.wav", m_pModel->GetLocation(), 0.5f);

        if (m_iLevel == 1)
        {
            m_pModel->TranslateOverTime(3, Ogre::Vector3(0,-15,0));
            m_pModel2->TranslateOverTime(3, Ogre::Vector3(0,-15,0));
        }
        else if (m_iLevel == 2)
        {
            m_pModel->TranslateOverTime(4.5, Ogre::Vector3(0,-15,0));
            m_pModel2->TranslateOverTime(4.5, Ogre::Vector3(0,-15,0));
        }

        m_eState = TS_UPGRADE;
    }
    else
    {
        // Set tower to build state and increase its level
        m_eState = TS_BUILD;
        m_iLevel++;

        SoundManager::StopSoundAtChannel(m_iChannelNumber);
        m_iChannelNumber = SoundManager::Play3DLoopedSound("buildingTools.wav", m_pTile->GetLocation(), 0.75f);
        
        // Adjust the stats and model of the tower for the new level
        if (m_iLevel == 2)
        {
            m_fBuildTimeLeft = 3;
        
            m_fRange = 2.25 * Tile::GetDefaultSize();
            m_fAtkRate = 1;
            m_fDamage = 200;
            m_fRadius = Tile::GetDefaultSize() * 0.75;
        
            m_v3ProjectileOffset = Ogre::Vector3(0,6,0);
            m_fProjectileSpeed = 75;

            // Set up the new model
            Deattach();
            m_pModel->CleanUp();
            m_pModel2->CleanUp();
            m_pModel = 0;
            m_pModel2 = 0;

            m_pModel =  new Model("Cannon3_2.mesh");
            m_pModel->Scale(Ogre::Vector3(2.5));
            m_pModel2 = new Model("Cannon3_1.mesh");
            m_pModel2->Scale(Ogre::Vector3(2.5));
            m_pModel2->SetAxis(RA_Y);

            // Move tower back to correct location
            m_pModel->Translate(m_pTile->GetLocation());
            m_pModel2->Translate(m_pTile->GetLocation());

            // fix position of seperate parts of the tower
            m_pModel->Translate(Ogre::Vector3(0,-9.0,0));
            m_pModel2->Translate(Ogre::Vector3(0,-2.2,0));

            // Raise the tower out of the ground for a build animation
            m_pModel->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,13,0));
            m_pModel2->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,13,0));

            // Get rid of old bounding box and set up the new one
            m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
            m_pBoundingBox->attachObject(BoundingBox::GenerateBoundingBox(m_pModel->GetSceneNode()->getName(), BT_TOWER));
            m_pBoundingBox->setVisible(false);
            m_pBoundingBox->translate(0,-1.55,0);
            m_pBoundingBox->scale(Ogre::Vector3(0.25,0.45,0.25));
            Attach();
        }
        else if (m_iLevel == 3)
        {
            m_fBuildTimeLeft = 4.5;
        
            m_fRange = 3.0 * Tile::GetDefaultSize();
            m_fAtkRate = 0.9;
            m_fDamage = 250;
            m_fRadius = Tile::GetDefaultSize();

            m_v3ProjectileOffset = Ogre::Vector3(0,7.5,0);
            m_fProjectileSpeed = 100;

            // Set up the new model
            Deattach();
            m_pModel->CleanUp();
            m_pModel2->CleanUp();
            m_pModel = 0;
            m_pModel2 = 0;

            m_pModel =  new Model("Cannon2_1.mesh");
            m_pModel->Scale(Ogre::Vector3(2.5));
            m_pModel2 = new Model("Cannon2_2.mesh");
            m_pModel2->Scale(Ogre::Vector3(1.0));
            m_pModel2->GetSceneNode()->roll(Ogre::Degree(90));
            m_pModel2->GetSceneNode()->pitch(Ogre::Degree(-90));
            m_pModel2->SetAxis(RA_Z);

            // Move tower back to correct location
            m_pModel->Translate(m_pTile->GetLocation());
            m_pModel2->Translate(m_pTile->GetLocation());

            // fix position of seperate parts of the tower
            m_pModel->Translate(Ogre::Vector3(0,-15,0));
            m_pModel2->Translate(Ogre::Vector3(0,-8.5,0));

            // Raise the tower out of the ground for a build animation
            m_pModel->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,20,0));
            m_pModel2->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,20,0));

            // Get rid of old bounding box and set up the new one
            m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
            m_pBoundingBox->attachObject(BoundingBox::GenerateBoundingBox(m_pModel->GetSceneNode()->getName(), BT_TOWER));
            m_pBoundingBox->setVisible(false);
            m_pBoundingBox->translate(0,-2.0,0);
            m_pBoundingBox->scale(Ogre::Vector3(0.25,0.6,0.25));
            Attach();
        }

        m_bUpgradeReady = false;
    }
}
//-----------------------------------------------------------------------------
// Salvage the tower, returning a portion of its resources to the player
// and destroying it as a result
//-----------------------------------------------------------------------------
void CannonTower::Salvage()
{
    m_iChannelNumber = SoundManager::Play3DLoopedSound("towerDescend.wav", m_pModel->GetLocation(), 0.75f);
    m_eState = TS_SOLD;
    m_fSellTimeLeft = 2.0f;
    SelectionCircle::Deselect(m_pModel->GetSceneNode());
    SetSelectable(false);
    MainCurrency::Refund(m_cCost[m_iLevel-1]);

    // Sink the tower back into the ground
    if (m_iLevel == 1)
    {
        m_pModel->TranslateOverTime(3, Ogre::Vector3(0,-15,0));
        m_pModel2->TranslateOverTime(3, Ogre::Vector3(0,-15,0));
    }
    else if (m_iLevel == 2)
    {
        m_pModel->TranslateOverTime(3, Ogre::Vector3(0,-15,0));
        m_pModel2->TranslateOverTime(3, Ogre::Vector3(0,-15,0));
    }
    else if (m_iLevel == 3)
    {
        m_pModel->TranslateOverTime(3, Ogre::Vector3(0,-15,0));
        m_pModel2->TranslateOverTime(3, Ogre::Vector3(0,-15,0));
    }
}
//-----------------------------------------------------------------------------
// Tower checks its current target
//-----------------------------------------------------------------------------
void CannonTower::Attack(Ogre::Real p_rTimeSinceLastFrame)
{
    // First check if we have a priority target and attack it if possible
    if (m_bHasPriorityTarget)
    {
        // Our priority target is not dead, so we check if it is in range
        if(!m_pPriorityTarget->CanDelete() &&
            m_pPriorityTarget->GetState() != DeadState)
        {
            // If it is range, we attack it
            if (m_pPriorityTarget->InRange(m_pModel->GetLocation(), m_fRange, m_fRangeModifier))
            {
                m_bPriorityTargetInRange = true;

                // Face the top of the tower to the enemy while targetting them
                m_pModel2->FaceTo(m_vEnemies.at(m_iCurrentTarget)->GetLocation());
                
                // Make sure enough time has passed between attacks before attacking again
                if (m_fTimeUntilNextAtk <= 0.0f)
                {
                    // create a projectile and fire it at the enemy's current position
                    ProjectileManager::Add(PT_CannonShot, m_pModel->GetLocation(), m_fProjectileSpeed, 
                                           m_vEnemies.at(m_iCurrentTarget), m_fDamage,
                                           m_v3ProjectileOffset, m_fRadius);
                    PlaySound();
                    m_fTimeUntilNextAtk = m_fAtkRate; // Reset attack timer to tower's attack speed
                }
            }
            else
                m_bPriorityTargetInRange = false;
        }
        // If our priority target is dead, we flag the tower for no priority target
        else
            m_bHasPriorityTarget = false;
    }
    // If our priority target is not in range or we don't have one, perform normal attack pattern
    if (!m_bHasPriorityTarget || !m_bPriorityTargetInRange)
    {
        // Check if we have iterated past the last enemy in our current target vector
        if (m_iCurrentTarget < m_vEnemies.size())
        {
            // Check that the enemy is still in range, not dead and not flagged for deletion
            if(!m_vEnemies.at(m_iCurrentTarget)->CanDelete() &&
                m_vEnemies.at(m_iCurrentTarget)->GetState() != DeadState &&
                m_vEnemies.at(m_iCurrentTarget)->InRange(m_pModel->GetLocation(), m_fRange, m_fRangeModifier))
            {   
                // Face the top of the tower to the enemy while targetting them
                m_pModel2->FaceTo(m_vEnemies.at(m_iCurrentTarget)->GetLocation());
                
                // Make sure enough time has passed between attacks before attacking again
                if (m_fTimeUntilNextAtk <= 0.0f)
                {
                    // create a projectile and fire it at the enemy's current position
                    ProjectileManager::Add(PT_CannonShot, m_pModel->GetLocation(), m_fProjectileSpeed, 
                                           m_vEnemies.at(m_iCurrentTarget), m_fDamage,
                                           m_v3ProjectileOffset, m_fRadius);
                    PlaySound();
                    m_fTimeUntilNextAtk = m_fAtkRate; // Reset attack timer to tower's attack speed
                }
            }
       	    else
                m_iCurrentTarget++; // Move to the next target
 	    }
 	    else
 	    {
       	    m_eState = TS_IDLE;
       	    m_iCurrentTarget = 0;
 	    }
    }
    
    /*
    if (m_iCurrentTarget < m_vEnemies.size())
    {
        if(!m_vEnemies.at(m_iCurrentTarget)->CanDelete() &&
            m_vEnemies.at(m_iCurrentTarget)->GetState() != DeadState &&
            m_vEnemies.at(m_iCurrentTarget)->InRange(m_pModel->GetLocation(), m_fRange))
        {
            // Face the top of the tower to the enemy while targetting them
            m_pModel2->FaceTo(m_vEnemies.at(m_iCurrentTarget)->GetLocation());
            
            // Make sure enough time has passed between attacks before attacking again
            if (m_fTimeUntilNextAtk <= 0.0f)
            {
                // create a projectile and fire it at the enemy's current position
                ProjectileManager::Add(PT_CannonShot, m_pModel->GetLocation(), m_fProjectileSpeed, 
                                       m_vEnemies.at(m_iCurrentTarget), m_fDamage,
                                       m_v3ProjectileOffset);
                m_fTimeUntilNextAtk = m_fAtkRate;
            }
        }
       	else
            m_iCurrentTarget++;
 	}
 	else
 	{
       	m_eState = TS_IDLE;
       	m_iCurrentTarget = 0;
 	}
    */
}
//-----------------------------------------------------------------------------
// Check the state of the cannon tower and perform an appropriate action
//-----------------------------------------------------------------------------
void CannonTower::Update(const Ogre::FrameEvent& p_feEvent)
{
	// Decrement timers
    if (m_fTimeUntilNextAtk > 0)
        m_fTimeUntilNextAtk -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

    // Check state and perform appropriate action
    if (m_eState == TS_FROZEN)
    {
        // Don't do anything here
    }
    if (m_eState == TS_ATTACK)  // attack an acquired target
    {
        Attack(p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed());
    }
    else if (m_eState == TS_IDLE) // check for enemies
    {
		m_pModel2->Rotate(25*p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed()); // rotate the top of the tower
        GetTargetsInRange();
    }
    else if (m_eState == TS_BUILD)  // continue build/upgrade until finished
    {
        m_fBuildTimeLeft -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();
        if (m_fBuildTimeLeft <= 0)
        {
            SoundManager::StopSoundAtChannel(m_iChannelNumber);
            SoundManager::Play3DSound("cannonTowerBuilt.wav", m_pTile->GetLocation());
            m_eState = TS_IDLE;         // build finished
            m_bCanInteract = true;
            SetSelectable(true);
            m_pBuildEffect->setEmitting(false);
        }
    }
    // Sink the tower into the ground before the new one starts building
    else if (m_eState == TS_UPGRADE)
    {
        // Check if the tower has finished sinking into the ground
        if (!m_pModel->IsTranslating())
        {
            m_bUpgradeReady = true;
            Upgrade();
        }
    }
    else if (m_eState == TS_SOLD)  // play death animation and destroy this instance
    {
        m_fSellTimeLeft -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

		/*****************************************************************************
		*	Set the object to not selectable if it is selectable (By Pierre)
		******************************************************************************/
		if(m_bIsSelectable)
		{
			SetSelectable(false);
		}
		/*****************************************************************************/

        if (m_fSellTimeLeft <= 0)  // if animation has finished, flag tower for deletion
        {
            SoundManager::StopSoundAtChannel(m_iChannelNumber);
            m_bCanDelete = true;
            m_pTile->SetHasTower(false);
        }
    }
    
    m_pModel->Update(p_feEvent);
    m_pModel2->Update(p_feEvent);
}

void CannonTower::PlaySound()
{
    int selection = rand() % 3;

    std::string fileList[3];

    fileList[0] = "cannonShoot1.wav";
    fileList[1] = "cannonShoot2.wav";
    fileList[2] = "cannonShoot3.wav";

    SoundManager::Play3DSound(fileList[selection], m_pModel2->GetLocation(), 0.25f);
}

void CannonTower::HandleEvent(const Hills::GameEvent& p_geGameEvent)
{
    if(p_geGameEvent.IsNight())
    {
        m_fRangeModifier = 0.65f;
    }
    else
    {
        m_fRangeModifier = 1.0f;
    }
}
