//-----------------------------------------------------------------------------
// ArrowTower.cpp                                           By: Keenan Johnston
// 
// Implementation file for ArrowTower.h
//-----------------------------------------------------------------------------

#include "MagicTower.h"

Currency MagicTower::m_cBuildCost[] = { Currency(100,0,0,0), Currency(160,10,0,0), Currency(240,5,3,0)};

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
MagicTower::MagicTower(Tile* p_pTile, bool p_bIsNight)
{
    // Set up tower stats
    m_fRange = 3.0 * Tile::GetDefaultSize();
    m_fAtkRate = 1.5;
    m_fDamage = 100;
    m_iLevel = 1;

    //Those wizards are blind man
    if(p_bIsNight)
    {
        m_fRangeModifier = 0.25f;
    }
    else
    {
        m_fRangeModifier = 1.0f;
    }

    // Costs for each level
    m_cCost[0] = Currency(100,0,0,0);
    m_cCost[1] = Currency(160,10,0,0);
    m_cCost[2] = Currency(240,5,3,0);

    // Projectile stats
    m_v3ProjectileOffset = Ogre::Vector3(0,22,0);
    m_fProjectileSpeed = 40;

    m_bCanInteract = false;
    m_bCanDelete = false;
    m_pTile = p_pTile;
    m_pTile->SetHasTower(true);

    // Selection circle parameters
    m_fCircleSize = 12.0f;
    m_fCircleOffset = 0.0f;
  
    // Create the visual representation of the tower
    m_pModel = new Model("MagicTower.mesh");
    m_pModel->Scale(Ogre::Vector3(3.0, 2.2, 3.0));

    // Move the tower to the build point
    m_pModel->TranslateTo(Ogre::Vector3(m_pTile->GetLocation().x, 
                                        m_pTile->GetLocation().y - 15,
                                        m_pTile->GetLocation().z));

    // Adjust tower's range based on its height
    m_fRange += (m_pTile->GetLocation().y - TileSet::GetPathHeight()) / 12 * Tile::GetDefaultSize();

    // Set up custom bounding box
    m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
    m_pBoundingBox->attachObject(BoundingBox::GenerateBoundingBox(m_pModel->GetSceneNode()->getName(), BT_TOWER));
    m_pBoundingBox->setVisible(false);
    //m_pBoundingBox->translate(-1.5,-4,0);
    m_pBoundingBox->scale(Ogre::Vector3(0.22,1.1,0.22));

    // Set tower state to build
    m_eState = TS_BUILD;
    m_fBuildTimeLeft = 2;
    m_bUpgradeReady = false;
    SetSelectable(false);

    // Particles
    m_pParticleSystem1 = Ogre::Root::getSingletonPtr()->
                         getSceneManager(BaseApplication::GetSceneManagerName())->
                         createParticleSystem(m_pModel->GetSceneNode()->getName() + "magic", "MagicGlow");
    m_pParticleSystem2 = Ogre::Root::getSingletonPtr()->
                         getSceneManager(BaseApplication::GetSceneManagerName())->
                         createParticleSystem(m_pModel->GetSceneNode()->getName() + "magiclines", "MagicLines");
    m_pParticleSystem3 = Ogre::Root::getSingletonPtr()->
                         getSceneManager(BaseApplication::GetSceneManagerName())->
                         createParticleSystem(m_pModel->GetSceneNode()->getName() + "magiclinesbig", "MagicLinesBig");

    // Stop them from emitting for now
    m_pParticleSystem1->setEmitting(false);
    m_pParticleSystem2->setEmitting(false);
    m_pParticleSystem3->setEmitting(false);

    Ogre::SceneNode* m_pSceneNode = Ogre::Root::getSingletonPtr()->
                                    getSceneManager(BaseApplication::GetSceneManagerName())->
                                    getRootSceneNode()->createChildSceneNode();
    m_pSceneNode->translate(Ogre::Vector3(m_pTile->GetLocation().x, 
                                          m_pTile->GetLocation().y + 22,
                                          m_pTile->GetLocation().z));
    m_pSceneNode->attachObject(m_pParticleSystem1);
    m_pSceneNode->attachObject(m_pParticleSystem2);
    m_pSceneNode->attachObject(m_pParticleSystem3);

    // Create and attach dust particle
    m_pBuildEffect = Ogre::Root::getSingletonPtr()->
                     getSceneManager(BaseApplication::GetSceneManagerName())->
                     createParticleSystem(m_pModel->GetSceneNode()->getName() + "Dust", "Dust");

    m_pDustNode = Ogre::Root::getSingletonPtr()->
                  getSceneManager(BaseApplication::GetSceneManagerName())->
                  getRootSceneNode()->createChildSceneNode();
    m_pDustNode->translate(m_pTile->GetLocation());
    m_pDustNode->attachObject(m_pBuildEffect);
    m_iChannelNumber = SoundManager::Play3DLoopedSound("buildingTools.wav", m_pTile->GetLocation());

    // Raise the tower out of the ground for a build animation
    m_pModel->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,15,0));
}

//-----------------------------------------------------------------------------
// Upgrade the tower to the next level
//-----------------------------------------------------------------------------
void MagicTower::Upgrade()
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
            m_pModel->TranslateOverTime(2, Ogre::Vector3(0,-19,0));
        else if (m_iLevel == 2)
            m_pModel->TranslateOverTime(3, Ogre::Vector3(0,-19,0));

        m_eState = TS_UPGRADE;
    }
    else
    {
        // Set tower to build state and increase its level
        m_eState = TS_BUILD;
        m_iLevel++;

        SoundManager::StopSoundAtChannel(m_iChannelNumber);
        m_iChannelNumber = SoundManager::Play3DLoopedSound("buildingTools.wav", m_pTile->GetLocation());
        
        // Adjust the stats and model of the tower for the new level
        if (m_iLevel == 2)
        {
            m_fBuildTimeLeft = 2;
        
            m_fRange = 3.5 * Tile::GetDefaultSize();
            m_fAtkRate = 0.4;
            m_fDamage = 115;
        
            m_v3ProjectileOffset = Ogre::Vector3(0,7.5,0);
            m_fProjectileSpeed = 144;

            // Set up the new model
            Deattach();
            m_pModel->CleanUp();
            m_pModel = 0;

            m_pModel =  new Model("Sniper2.mesh");
            m_pModel->Scale(Ogre::Vector3(1.0));

            // Move tower back to correct location
            m_pModel->TranslateTo(Ogre::Vector3(m_pTile->GetLocation().x, 
                                                m_pTile->GetLocation().y - 15, 
                                                m_pTile->GetLocation().z));

            // Raise the tower out of the ground for a build animation
            m_pModel->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,19,0));

            // Get rid of old bounding box and set up the new one
            m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
            m_pBoundingBox->attachObject(BoundingBox::GenerateBoundingBox(m_pModel->GetSceneNode()->getName(), BT_TOWER));
            m_pBoundingBox->setVisible(false);
            m_pBoundingBox->translate(0,-4,0);
            m_pBoundingBox->scale(Ogre::Vector3(0.65,1.65,0.65));
            Attach();
        }
        else if (m_iLevel == 3)
        {
            m_fBuildTimeLeft = 3;
        
            m_fRange = 4.0 * Tile::GetDefaultSize();
            m_fAtkRate = 0.3;
            m_fDamage = 135;

            m_v3ProjectileOffset = Ogre::Vector3(0,7.5,0);
            m_fProjectileSpeed = 168;

            // Set up the new model
            Deattach();
            m_pModel->CleanUp();
            m_pModel = 0;

            m_pModel =  new Model("Sniper3.mesh");
            m_pModel->Scale(Ogre::Vector3(1.0));

            // Move tower back to correct location
            m_pModel->TranslateTo(Ogre::Vector3(m_pTile->GetLocation().x + 1.5, 
                                                m_pTile->GetLocation().y - 15, 
                                                m_pTile->GetLocation().z));

            // Raise the tower out of the ground for a build animation
            m_pModel->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,19,0));

            // Get rid of old bounding box and set up the new one
            m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
            m_pBoundingBox->attachObject(BoundingBox::GenerateBoundingBox(m_pModel->GetSceneNode()->getName(), BT_TOWER));
            m_pBoundingBox->setVisible(false);
            m_pBoundingBox->translate(-1.5,-4,0);
            m_pBoundingBox->scale(Ogre::Vector3(0.65,1.65,0.65));
            Attach();
        }

        m_bUpgradeReady = false;
    }
}

//-----------------------------------------------------------------------------
// Salvage the tower, returning a portion of its resources to the player
// and destroying it as a result
//-----------------------------------------------------------------------------
void MagicTower::Salvage()
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
void MagicTower::Attack(Ogre::Real p_rTimeSinceLastFrame)
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
                
                // Make sure enough time has passed between attacks before attacking again
                if (m_fTimeUntilNextAtk <= 0.0f)
                {
                    // create a projectile and fire it at the enemy's current position
                    ProjectileManager::Add(PT_ArrowShot, m_pModel->GetLocation(), m_fProjectileSpeed, 
                                           m_pPriorityTarget, m_fDamage, m_v3ProjectileOffset);
                    SoundManager::Play3DSound("magicShot.wav", m_pModel->GetLocation());
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
                // Make sure enough time has passed between attacks before attacking again
                if (m_fTimeUntilNextAtk <= 0.0f)
                {
                    // create a projectile and fire it at the enemy's current position
                    ProjectileManager::Add(PT_MAGIC, m_pModel->GetLocation(), m_fProjectileSpeed, 
                                           m_vEnemies.at(m_iCurrentTarget), m_fDamage,
                                           m_v3ProjectileOffset);
                    SoundManager::Play3DSound("magicShot.wav", m_pModel->GetLocation());
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
}

//-----------------------------------------------------------------------------
// Check the state of the cannon tower and perform an appropriate action
//-----------------------------------------------------------------------------
void MagicTower::Update(const Ogre::FrameEvent& p_feEvent)
{
	// Decrement timers
    if (m_fTimeUntilNextAtk > 0)
        m_fTimeUntilNextAtk -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

    // Check state and perform appropriate action
    if (m_eState == TS_FROZEN)
    {
        // Turn particles off
        m_pParticleSystem1->setEmitting(false);
        m_pParticleSystem2->setEmitting(false);
        m_pParticleSystem3->setEmitting(false);
    }
    if (m_eState == TS_ATTACK)  // attack an acquired target
    {
        m_pParticleSystem1->setEmitting(true);
        m_pParticleSystem2->setEmitting(false);
        m_pParticleSystem3->setEmitting(true);
        Attack(p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed());
    }
    else if (m_eState == TS_IDLE) // check for enemies
    {
        GetTargetsInRange();
        m_pParticleSystem1->setEmitting(true);
        m_pParticleSystem2->setEmitting(true);
        m_pParticleSystem3->setEmitting(false);
    }
    else if (m_eState == TS_BUILD)  // continue build/upgrade until finished
    {
        m_fBuildTimeLeft -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();
        if (m_fBuildTimeLeft <= 0)
        {
            SoundManager::StopSoundAtChannel(m_iChannelNumber);
            SoundManager::Play3DSound("magicTowerBuilt.wav", m_pTile->GetLocation());
            m_eState = TS_IDLE;         // build finished
            m_bCanInteract = true;
            SetSelectable(true);
            m_pBuildEffect->setEmitting(false);
            // Start particles
            m_pParticleSystem1->setEmitting(true);
            m_pParticleSystem2->setEmitting(true);
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

void MagicTower::HandleEvent(const Hills::GameEvent& p_geGameEvent)
{
    if(p_geGameEvent.IsNight())
    {
        m_fRangeModifier = 0.25f;
    }
    else
    {
        m_fRangeModifier = 1.0f;
    }
}