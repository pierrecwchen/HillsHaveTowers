//-----------------------------------------------------------------------------
// TeslaTower.cpp                                           By: Keenan Johnston
// 
// Implementation file for BurstTower.h
//-----------------------------------------------------------------------------

#include "TeslaTower.h"

Currency TeslaTower::m_cBuildCost[] = { Currency(175,3,0,0) };

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
TeslaTower::TeslaTower(Tile* p_pTile, bool p_bIsNight)
{
    // Set up tower stats
    m_fRange = 2.0 * Tile::GetDefaultSize();
    m_fAtkRate = 1.0;
    m_fDamage = 200;
    m_iJumpCount = 3;
    m_iLevel = 1;

    //Electricity works the same way at night as it does at day
    m_fRangeModifier = 1.0f;

    m_cCost[0] = Currency(175,3,0,0);
    m_cCost[1] = Currency(300,0,0,3);
    m_cCost[2] = Currency(500,5,5,5);

    m_bCanInteract = false;
    m_bCanDelete = false;
    m_pTile = p_pTile;
    m_pTile->SetHasTower(true);

    m_fCircleSize = 10.0f;
    m_fCircleOffset = 0.0f;
  
    // Create the visual representation of the tower
    m_pModel =  new Model("Burst1_1.mesh");
    m_pModel->Scale(Ogre::Vector3(1.0));
    m_pModel2 =  new Model("Teslaball.mesh");
    m_pModel2->Scale(Ogre::Vector3(2.8));
    m_pModel3 = new Model("MetalBall.mesh");
    m_pModel3->Scale(Ogre::Vector3(0.8));

    // Move tower to build location
    m_pModel->Translate(m_pTile->GetLocation());
    m_pModel2->Translate(m_pTile->GetLocation());
    m_pModel3->Translate(m_pTile->GetLocation());

    m_pModel->Translate(Ogre::Vector3(0,-15,0));  // fix position of seperate parts of the tower
    m_pModel2->Translate(Ogre::Vector3(0,-9.7,0));
    m_pModel3->Translate(Ogre::Vector3(0,-9.7,0));

    // Set tower state to build
    m_eState = TS_BUILD;
    m_fBuildTimeLeft = 3;
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

    // Get rid of old bounding box and set up the new one
    m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
    m_pBoundingBox->attachObject(BoundingBox::GenerateBoundingBox(m_pModel->GetSceneNode()->getName(), BT_TOWER));
    m_pBoundingBox->setVisible(false);
    m_pBoundingBox->translate(0,0,0);
    m_pBoundingBox->scale(Ogre::Vector3(0.65,1.4,0.65));

    m_iChannelNumber = SoundManager::Play3DLoopedSound("buildingTools.wav", m_pTile->GetLocation());

    // Test
    Ogre::ParticleSystem* p1x = Ogre::Root::getSingletonPtr()->
                                getSceneManager(BaseApplication::GetSceneManagerName())->
                                createParticleSystem(m_pModel->GetSceneNode()->getName() + "static", "Lightning");
    m_pModel2->GetSceneNode()->attachObject(p1x);

    // Raise the tower out of the ground for a build animation
    m_pModel->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,15,0));
    m_pModel2->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,21.5,0));
    m_pModel3->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,21.5,0));
}

//-----------------------------------------------------------------------------
// Upgrade the tower to the next level
//-----------------------------------------------------------------------------
void TeslaTower::Upgrade()
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
        m_iChannelNumber = SoundManager::Play3DLoopedSound("buildingTools.wav", m_pTile->GetLocation());

        // Adjust the stats and model of the tower for the new level
        if (m_iLevel == 2)
        {
            m_fBuildTimeLeft = 5.5;
        
            m_fRange = 1.5 * Tile::GetDefaultSize();
            m_fAtkRate = 0.05;
            m_fDamage = 15;

            // Set up the new model
            m_pModel->CleanUp();
            m_pModel2->CleanUp();
            m_pModel = 0;
            m_pModel2 = 0;

            m_pModel =  new Model("Burst3_2.mesh");
            m_pModel->Scale(Ogre::Vector3(0.7));
            m_pModel2 = new Model("Burst3_1.mesh");
            m_pModel2->Scale(Ogre::Vector3(1.3));
            m_pModel2->GetSceneNode()->yaw(Ogre::Degree(-90));

            // Setup the particle effect again
            m_pParticleNode = m_pModel2->GetSceneNode()->createChildSceneNode();
            m_pParticleNode->attachObject(m_pParticleSystem);
            m_pParticleNode->scale(0.12, 0.12, 0.12);
            m_pParticleNode->translate(Ogre::Vector3(5, -0.5, 0));
            m_pParticleSystem->setEmitting(false);
       
            // Move tower back to correct location
            m_pModel->Translate(m_pTile->GetLocation());
            m_pModel2->Translate(m_pTile->GetLocation());

            // fix position of seperate parts of the tower
            m_pModel->Translate(Ogre::Vector3(-1.1,-15,1.2));
            m_pModel2->Translate(Ogre::Vector3(-0.1,-9.7,0.1));

            // Raise the tower out of the ground for a build animation
            m_pModel->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,21,0));
            m_pModel2->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,25.5,0));

            // Get rid of old bounding box and set up the new one
            m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
            m_pBoundingBox->attachObject(BoundingBox::GenerateBoundingBox(m_pModel->GetSceneNode()->getName(), BT_TOWER));
            m_pBoundingBox->setVisible(false);
            m_pBoundingBox->translate(1.5,-8.5,-1.7);
            m_pBoundingBox->scale(Ogre::Vector3(0.9,2.4,0.9));
            Attach();
        }
        else if (m_iLevel == 3)
        {
            m_fBuildTimeLeft = 8;
        
            m_fRange = 2.0 * Tile::GetDefaultSize();
            m_fAtkRate = 0.05;
            m_fDamage = 25;

            // Set up the new model
            m_pModel->CleanUp();
            m_pModel2->CleanUp();
            m_pModel = 0;
            m_pModel2 = 0;

            m_pModel =  new Model("Burst3_2.mesh");
            m_pModel->Scale(Ogre::Vector3(1.0));
            m_pModel2 = new Model("Burst3_1.mesh");
            m_pModel2->Scale(Ogre::Vector3(0.7));

            // Move tower back to correct location
            m_pModel->Translate(m_pTile->GetLocation());
            m_pModel2->Translate(m_pTile->GetLocation());

            // fix position of seperate parts of the tower
            m_pModel->Translate(Ogre::Vector3(0,-15,0));
            m_pModel2->Translate(Ogre::Vector3(0,-9.7,0));

            // Raise the tower out of the ground for a build animation
            m_pModel->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,15,0));
            m_pModel2->TranslateOverTime(m_fBuildTimeLeft, Ogre::Vector3(0,19.5,0));

           // Get rid of old bounding box and set up the new one
            m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
            m_pBoundingBox->attachObject(BoundingBox::GenerateBoundingBox(m_pModel->GetSceneNode()->getName(), BT_TOWER));
            m_pBoundingBox->setVisible(false);
            m_pBoundingBox->translate(0,0,0);
            m_pBoundingBox->scale(Ogre::Vector3(0.65,1.4,0.65));
            Attach();
        }

        m_bUpgradeReady = false;
    }
}
//-----------------------------------------------------------------------------
// Salvage the tower, returning a portion of its resources to the player
// and destroying it as a result
//-----------------------------------------------------------------------------
void TeslaTower::Salvage()
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
void TeslaTower::Attack(Ogre::Real p_rTimeSinceLastFrame)
{
    if (m_iCurrentTarget < m_vEnemies.size())
    {
        if(!m_vEnemies.at(m_iCurrentTarget)->CanDelete() &&
            m_vEnemies.at(m_iCurrentTarget)->GetState() != DeadState &&
            m_vEnemies.at(m_iCurrentTarget)->InRange(m_pModel->GetLocation(), m_fRange))
        {
            // Make sure enough time has passed between attacks before attacking again
            if (m_fTimeUntilNextAtk <= 0.0f)
            {
                // Fire a lightning bolt at the target
                ProjectileManager::Add(PT_LIGHTNING,
                                       m_pModel->GetLocation(), 
                                       0.0f, 
                                       m_vEnemies.at(m_iCurrentTarget), 
                                       m_fDamage,
                                       Ogre::Vector3(0,11.5,0),
                                       0.0f,
                                       0.0f,
                                       m_iJumpCount);
                int selection = rand() % 2;

                if(selection == 0)
                {
                    SoundManager::Play3DSound("teslaShot1.wav", m_pModel3->GetLocation(), 0.5f);
                }
                else
                {
                    SoundManager::Play3DSound("teslaShot2.wav", m_pModel3->GetLocation(), 0.5f);
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
void TeslaTower::Update(const Ogre::FrameEvent& p_feEvent)
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
            SoundManager::Play3DSound("teslaTowerBuilt.wav", m_pTile->GetLocation());
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
    m_pModel3->Update(p_feEvent);
}