//-----------------------------------------------------------------------------
// BaseTower.cpp                                            By: Keenan Johnston
// 
// Implementation file for BaseTower.h
//-----------------------------------------------------------------------------

#include "BaseTower.h"

//-----------------------------------------------------------------------------
// Empty constructor to set some default values
//-----------------------------------------------------------------------------
BaseTower::BaseTower()
    :
    m_pModel(0),
    m_pModel2(0),
    m_pBuildEffect(0),
    m_pDustNode(0),
    m_iLevel(1),
    m_bCanDelete(false),
    m_bAnimationPlaying(false),
    m_iCurrentTarget(0),
    m_fTimeUntilNextAtk(0),
    m_fParticleTime(0),
    m_bParticleActive(false),
    m_bHasPriorityTarget(false),
    m_bPriorityTargetInRange(false)
{
    /*
    m_pModel = 0;
    m_pModel2 = 0;
    m_pBuildEffect = 0;
    m_iLevel = 1;
    m_bCanDelete = false;
    m_bAnimationPlaying = false;
    m_iCurrentTarget = 0;
    m_fTimeUntilNextAtk = 0;
    m_fParticleTime = 0;
    m_bParticleActive = false;
    m_bHasPriorityTarget = false;
    m_bPriorityTargetInRange = false;
    */
}

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
BaseTower::BaseTower(float p_fRange, float p_fAtkRate, float p_fDamage, float p_fBuildTime,
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
// Destructor
//-----------------------------------------------------------------------------
BaseTower::~BaseTower()
{
    delete m_pModel;
}

//-----------------------------------------------------------------------------
// Upgrade the tower to the next level with the given stats
//-----------------------------------------------------------------------------
void BaseTower::Upgrade()
{
    // Set tower to idle build state
    m_eState = TS_BUILD;
    m_iLevel++;
    
    // Differentiate tower levels slightly (placeholder)
    m_pModel->Scale(Ogre::Vector3(1.0f,1.2f,1.0f));
}

//-----------------------------------------------------------------------------
// Salvage the tower, returning a portion of its resources to the player
// and destroying it as a result
//-----------------------------------------------------------------------------
void BaseTower::Salvage()
{
    m_eState = TS_SOLD;
    m_fSellTimeLeft = 2.0f;
    MainCurrency::Refund(m_cCost[m_iLevel-1]);
}

//-----------------------------------------------------------------------------
// Freeze the tower, making it unable to attack or animate until it is
// repaired by the player.
//-----------------------------------------------------------------------------
void BaseTower::Freeze()
{
    m_eState = TS_FROZEN;
    // Start a particle here
}

//-----------------------------------------------------------------------------
// Unfreeze the tower, letting it resume its normal actions.
//-----------------------------------------------------------------------------
void BaseTower::Unfreeze()
{
    m_eState = TS_IDLE;
}

//-----------------------------------------------------------------------------
// Set the priority target for this tower - it will always be attacked if it
// is range
//-----------------------------------------------------------------------------
void BaseTower::SetPriorityTarget(BaseEnemy* p_pEnemy)
{
    m_pPriorityTarget = p_pEnemy;
    m_bHasPriorityTarget = true;
}

//-----------------------------------------------------------------------------
// Check for any enemies within the range of the tower and add them to the
// target list, then set the tower into attack state
//-----------------------------------------------------------------------------
void BaseTower::GetTargetsInRange()
{
    if (EnemyManager::Instance()->EnemiesInRange(m_pModel->GetLocation(), m_fRange, m_fRangeModifier))
 	{
       	m_vEnemies = EnemyManager::Instance()->GetEnemiesInRange(m_pModel->GetLocation(), m_fRange, m_fRangeModifier);
       	m_eState = TS_ATTACK;
 	}
}

//-----------------------------------------------------------------------------
// Tower checks its current target and attacks if it can
//-----------------------------------------------------------------------------
void BaseTower::Attack(Ogre::Real p_rTimeSinceLastFrame)
{
    if (m_iCurrentTarget < m_vEnemies.size())
    {
        if(m_vEnemies.at(m_iCurrentTarget)->GetState() != DeadState &&
           m_vEnemies.at(m_iCurrentTarget)->InRange(m_pModel->GetLocation(), m_fRange))
        {
            if (m_fTimeUntilNextAtk <= 0.0f)
            {
                m_vEnemies.at(m_iCurrentTarget)->Attacked(m_fDamage, false, AT_STANDARD);
                m_fTimeUntilNextAtk = m_fAtkRate;
            }
            else
                m_fTimeUntilNextAtk -= p_rTimeSinceLastFrame; // needs to get this from frame event
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
// Use OGRE to delete model-related data before destroying this object
//-----------------------------------------------------------------------------
void BaseTower::DestroyModel()
{
	m_pTile = 0;
	m_pParticleSystem = 0;
	m_eCurrentTarget = 0;
	m_pPriorityTarget = 0;

    m_pModel->CleanUp();
    if (m_pModel2 != 0) 
        m_pModel2->CleanUp();
	delete m_pModel;
}

//-----------------------------------------------------------------------------
// Check the state of the tower and perform an appropriate action
//-----------------------------------------------------------------------------
void BaseTower::Update(const Ogre::FrameEvent& p_feEvent)
{
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
            m_eState = TS_IDLE;         // build finished
    }
    else if (m_eState == TS_SOLD)  // play death animation and destroy this instance
    {
        m_fSellTimeLeft -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

		/*****************************************************************************
		*	Cannot be selected and cannot recieve events in sold state. (By Pierre)
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

        if (m_fSellTimeLeft == 0)  // if animation has finished, flag tower for deletion
        {
            m_bCanDelete = true;
        }
    }
    
    m_pModel->Update(p_feEvent);
}

//-----------------------------------------------------------------------------
// Handle selection by the selection circle
//-----------------------------------------------------------------------------
void BaseTower::Select()
{
    SelectionCircle::LockTo(m_pModel->GetSceneNode(), m_fRange*2.1, m_fCircleOffset);
}

/**************************************************************************************
*	Show the bounding box of the entity. (Added by Pierre)
***************************************************************************************/
void BaseTower::ShowObject(bool p_bIsShow)
{
	// If it not show the bounding box but it want to show the box, show the box
	if(p_bIsShow && m_pModel != 0 && !m_bIsShow)
	{
		//m_pBoundingBox->setVisible(true);
        SelectionCircle::HoverOver(m_pModel->GetSceneNode(), m_fCircleSize, m_fCircleOffset);
		m_bIsShow = true;
	}

	// If it shows the bounding box but it don't want to show the box, disable it
	else if (!p_bIsShow && m_pModel != 0 && m_bIsShow)
	{
		//m_pBoundingBox->setVisible(false);
        SelectionCircle::HideHoverCircle();
		m_bIsShow = false;
	}
}

/**************************************************************************************
*	Handle the events. (Added by Pierre)
***************************************************************************************/
void BaseTower::HandleEvent(const Hills::GameEvent& p_geGameEvent)
{
	// Handle being frozen by the boss's attack
    if (p_geGameEvent.GetGameEventType() == Hills::GE_FreezeAttack)
    {
		Ogre::Vector3 loc = m_pModel->GetLocation();
		float inRange = sqrt(pow(loc.x - p_geGameEvent.GetOrigin().x, 2) + 
                             pow(loc.z - p_geGameEvent.GetOrigin().z, 2));
		
        if (inRange <= p_geGameEvent.GetRadius())  // Arbitrarily defined range
        {
            Freeze();
        }
    }
}