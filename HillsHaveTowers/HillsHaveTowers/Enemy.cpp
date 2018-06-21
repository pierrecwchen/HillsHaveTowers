//-----------------------------------------------------------------------------
// Enemy.cpp                                                By: Keenan Johnston
// 
// Implementation file for Enemy.h
//-----------------------------------------------------------------------------

#include "Enemy.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Enemy::Enemy(int p_iHp, int p_iCarry, float p_fArmour, float p_fSpeed,
             string p_sName, string p_sMesh, Ogre::SceneManager* p_smManager)
{
	// Set up enemy stats
	m_iHealth = p_iHp;
	m_iVillagersCarried = p_iCarry;
	m_fDmgReduction = p_fArmour;
	m_fWalkSpeed = p_fSpeed;
	
    // Not sure if this will work currently, as stuff gets added and deleted to the
    // vector there may be duplicate numbers happening
    std::stringstream ss;
    ss << (int)enemies->size();  // this won't work right now, enemy doesn't know the enemy vector
    p_sName += ss.str();         // size of vector could be passed as a parameter instead
    string nodeName = p_sName + "Node";
  
    // Need to have access to scene manager here to register new entity and node
	m_enEntity = p_smManager->createEntity(p_sName, p_sMesh);
    m_snNode = p_smManager->getRootSceneNode()->createChildSceneNode(nodeName);
    m_snNode->attachObject(m_enEntity);
	
	// Set spawn location of enemy, need to find this out from world?
    m_snNode->setPosition(/*get start point of path*/);
	
	// Set up the basic animation for the enemy
	m_asAnimationState = m_enEntity->getAnimationState("Walk");
	m_asAnimationState->setLoop(true);
	m_asAnimationState->setEnabled(true);
}
//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
Enemy::~Enemy()
{
    delete m_snNode;
	delete m_enEntity;
	delete m_asAnimationState;
}
//-----------------------------------------------------------------------------
// Decrement reference count of this object
//-----------------------------------------------------------------------------
void Enemy::DecrementReference()
{
    if (m_iRefCount > 0)
        m_iRefCount--;
    if (m_iRefCount == 0)
        delete (Enemy*)this;
}
//-----------------------------------------------------------------------------
// Enemy is attacked is attacked by a tower
//-----------------------------------------------------------------------------
void Enemy::Attacked(float power, bool pierce)
{
    // Play some sort of animation or effect depending on type of attack?
	if (pierce)
        m_iHealth -= power;
	else
        m_iHealth -= power * (1-dmgReduction);
}
//-----------------------------------------------------------------------------
// Determine if this enemy is (still) within range of a given tower
//-----------------------------------------------------------------------------
bool Enemy::InRange(Ogre::Vector3 p_v3Loc, float p_fRange)
{
    bool inRange = false;
    
    Ogre::Vector3 loc = m_snNode->getPosition();
	float inRange = (loc.x - p_v3Loc.x)^2 + (loc.y - p_v3Loc.y)^2;
    
    if (inRange <= range)
        inRange = true;
        
    return inRange;
}
//-----------------------------------------------------------------------------
// Update this object
//-----------------------------------------------------------------------------
void Enemy::Update(const Ogre::FrameEvent& p_feEvent)
{
	// If an enemy has no health left, put them in the death state
	if (m_iHealth <= 0)
    {
		m_eState = "dead";
        m_fDeathTimeLeft = 5.0f;  // placeholder
        
        m_asAnimationState = m_enEntity->getAnimationState("Die");
		m_asAnimationState->setLoop(false);
		m_asAnimationState->setEnabled(true);
    }
	else if (m_eState = "dead")  // Play animation until finished then destroy this instance
	{
        m_fDeathTimeLeft -= p_feEvent.timeSinceLastFrame;
        
        if (m_fDeathTimeLeft <= 0.0f)
            EnemyManager::Delete((Enemy*)this);
	}	
    else if (m_eState = "walk")  // Continue walking along the path
    {
        m_snNode->translate();  // have to determine where to walk
    }
	
	// Update the frame of animation of the enemy
	m_asAnimationState->addTime(p_feEvent.timeSinceLastFrame);
}