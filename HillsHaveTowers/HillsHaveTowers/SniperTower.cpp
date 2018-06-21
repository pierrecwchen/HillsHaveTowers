//-----------------------------------------------------------------------------
// SniperTower.cpp                                          By: Keenan Johnston
// 
// Implementation file for SniperTower.h
//-----------------------------------------------------------------------------

#include "SniperTower.h"

Currency SniperTower::m_cBuildCost[] = { Currency(200,5,0,0) };

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
SniperTower::SniperTower(Tile* p_pTile, bool p_bIsNight)
{
    // Set up tower stats
    m_fAtkRate = 2.5;
    m_fRange = 5.0 * Tile::GetDefaultSize();
    m_fDamage = 500;
    m_iLevel = 1;

    //I wanted to say that the sniper has like nightvision goggles or something but I feel as
    // though the sniper tower is powerful enough
    if(p_bIsNight)
    {
        m_fRangeModifier = 0.5f;
    }
    else
    {
        m_fRangeModifier = 1.0f;
    }

    m_cCost[0] = Currency(200,5,0,0);
    m_cCost[1] = Currency(400,15,0,0);
    m_cCost[2] = Currency(800,0,10,5);
    
    m_bCanInteract = false;
    m_bCanDelete = false;
    m_pTile = p_pTile;
    m_pTile->SetHasTower(true);

    m_fCircleSize = 10.0f;
    m_fCircleOffset = -1.5f;
  
    // Create the visual representation of the tower
    m_pModel = new Model("Sniper1.mesh");
    m_pModel->Scale(Ogre::Vector3(1.2));

    // Move the tower to the build point
    m_pModel->TranslateTo(Ogre::Vector3(m_pTile->GetLocation().x + 1.3, 
                                        m_pTile->GetLocation().y - 16,
                                        m_pTile->GetLocation().z));

    // Adjust tower's range based on its height
    m_fRange += (m_pTile->GetLocation().y - TileSet::GetPathHeight()) / 12 * Tile::GetDefaultSize();

    // Set up custom bounding box
    m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
    m_pBoundingBox->attachObject(BoundingBox::GenerateBoundingBox(m_pModel->GetSceneNode()->getName(), BT_TOWER));
    m_pBoundingBox->setVisible(false);
    m_pBoundingBox->translate(-1.5,-10.0,0);
    m_pBoundingBox->scale(Ogre::Vector3(0.65,2.3,0.65));

    // Set tower state to build
    m_eState = TS_BUILD;
    m_fBuildTimeLeft = 8;
    m_bUpgradeReady = false;

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
    m_pModel->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,28,0));
}

//-----------------------------------------------------------------------------
// Old constructor
//-----------------------------------------------------------------------------
SniperTower::SniperTower(float p_fRange, float p_fAtkRate, float p_fDamage, float p_fBuildTime,
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
void SniperTower::Upgrade()
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
            m_pModel->TranslateOverTime(6, Ogre::Vector3(0,-28,0));
        else if (m_iLevel == 2)
            m_pModel->TranslateOverTime(8, Ogre::Vector3(0,-28,0));

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
            m_fBuildTimeLeft = 6;
        
            m_fRange = 6.0 * Tile::GetDefaultSize();
            m_fAtkRate = 2.25;
            m_fDamage = 720;
        
            m_v3ProjectileOffset = Ogre::Vector3(0,7.5,0);
            m_fProjectileSpeed = 1200;

            // Set up the new model
            SetSelectable(false);
            Deattach();
            m_pModel->CleanUp();
            m_pModel = 0;

            m_pModel =  new Model("Sniper2.mesh");
            m_pModel->Scale(Ogre::Vector3(1.2));

            // Move tower back to correct location
            m_pModel->TranslateTo(Ogre::Vector3(m_pTile->GetLocation().x, 
                                                m_pTile->GetLocation().y - 16, 
                                                m_pTile->GetLocation().z));

            // Raise the tower out of the ground for a build animation
            m_pModel->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,28.5,0));

            // Get rid of old bounding box and set up the new one
            m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
            m_pBoundingBox->attachObject(BoundingBox::GenerateBoundingBox(m_pModel->GetSceneNode()->getName(), BT_TOWER));
            m_pBoundingBox->setVisible(false);
            m_pBoundingBox->translate(0,-10.3,0);
            m_pBoundingBox->scale(Ogre::Vector3(0.65,2.3,0.65));
            SetSelectable(true);
            Attach();
        }
        else if (m_iLevel == 3)
        {
            m_fBuildTimeLeft = 8;
        
            m_fRange = 7.0 * Tile::GetDefaultSize();
            m_fAtkRate = 2;
            m_fDamage = 1000;

            m_v3ProjectileOffset = Ogre::Vector3(0,7.5,0);
            m_fProjectileSpeed = 1400;

            // Set up the new model
            SetSelectable(false);
            Deattach();
            m_pModel->CleanUp();
            m_pModel = 0;

            m_pModel =  new Model("Sniper3.mesh");
            m_pModel->Scale(Ogre::Vector3(1.2));

            // Move tower back to correct location
            m_pModel->TranslateTo(Ogre::Vector3(m_pTile->GetLocation().x + 1.5, 
                                                m_pTile->GetLocation().y - 16, 
                                                m_pTile->GetLocation().z));

            // Raise the tower out of the ground for a build animation
            m_pModel->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,28,0));

            // Get rid of old bounding box and set up the new one
            m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
            m_pBoundingBox->attachObject(BoundingBox::GenerateBoundingBox(m_pModel->GetSceneNode()->getName(), BT_TOWER));
            m_pBoundingBox->setVisible(false);
            m_pBoundingBox->translate(-1.5,-10.0,0);
            m_pBoundingBox->scale(Ogre::Vector3(0.65,2.3,0.65));
            SetSelectable(true);
            Attach();
        }

        m_bUpgradeReady = false;
    }
}
//-----------------------------------------------------------------------------
// Salvage the tower, returning a portion of its resources to the player
// and destroying it as a result
//-----------------------------------------------------------------------------
void SniperTower::Salvage()
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
        m_pModel->TranslateOverTime(2, Ogre::Vector3(0,-28,0));
    }
    else if (m_iLevel == 2)
    {
        m_pModel->TranslateOverTime(2, Ogre::Vector3(0,-28,0));
    }
    else if (m_iLevel == 3)
    {
        m_pModel->TranslateOverTime(2, Ogre::Vector3(0,-28,0));
    }
}
//-----------------------------------------------------------------------------
// Tower checks its current target
//-----------------------------------------------------------------------------
void SniperTower::Attack(Ogre::Real p_rTimeSinceLastFrame)
{
    if (m_iCurrentTarget < m_vEnemies.size())
    {
        if(!m_vEnemies.at(m_iCurrentTarget)->CanDelete() &&
            m_vEnemies.at(m_iCurrentTarget)->GetState() != DeadState &&
            m_vEnemies.at(m_iCurrentTarget)->InRange(m_pModel->GetLocation(), m_fRange, m_fRangeModifier))
        {
            // Make sure enough time has passed between attacks before attacking again
            if (m_fTimeUntilNextAtk <= 0.0f)
            {
                // create a projectile and fire it at the enemy's current position
                ProjectileManager::Add(PT_SniperShot, m_pModel->GetLocation(), 12000, 
                                       m_vEnemies.at(m_iCurrentTarget), m_fDamage,
                                       Ogre::Vector3(-1.8,11.5,0));
                if(m_iLevel == 1)
                {
                    SoundManager::Play3DSound("sniperShot1.wav", m_pModel->GetLocation(), 0.35f);
                }
                else if (m_iLevel == 2)
                {
                    SoundManager::Play3DSound("sniperShot2.wav", m_pModel->GetLocation(), 0.35f);
                }
                else if (m_iLevel == 3)
                {
                    SoundManager::Play3DSound("sniperShot3.wav", m_pModel->GetLocation(), 0.35f);
                }
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
}
//-----------------------------------------------------------------------------
// Check the state of the cannon tower and perform an appropriate action
//-----------------------------------------------------------------------------
void SniperTower::Update(const Ogre::FrameEvent& p_feEvent)
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
        GetTargetsInRange();
    }
    else if (m_eState == TS_BUILD)  // continue build/upgrade until finished
    {
        m_fBuildTimeLeft -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();
        if (m_fBuildTimeLeft <= 0)
        {
            SoundManager::StopSoundAtChannel(m_iChannelNumber);
            SoundManager::Play3DSound("sniperTowerBuilt.wav", m_pTile->GetLocation());
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
}

void SniperTower::HandleEvent(const Hills::GameEvent& p_geGameEvent)
{
    if(p_geGameEvent.IsNight())
    {
        m_fRangeModifier = 0.5f;
    }
    else
    {
        m_fRangeModifier = 1.0f;
    }
}