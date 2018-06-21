//-----------------------------------------------------------------------------
// Soldier.cpp                                              By: Keenan Johnston
// 
// Implementation file for Soldier.h
//-----------------------------------------------------------------------------

#include "Soldier.h"
#include "SoundManager.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Soldier::Soldier(std::list<Tile> p_lShortestPath, float p_fXOffset, float p_fZOffset)
{
    // Set up soldier stats
	m_iCurrentHealth = m_iMaxHealth = 4000;
	m_iVillagersCarried = 1;
	m_fDmgReduction = 0.15f;
	m_fWalkSpeed = 1;

    m_bHasArmor = false;
    m_bIdentified = false;

    m_fScreamPitch = 1.0f;

    m_cLoot = Currency(20,0,0,0);

    m_fCircleSize = 9.0f;
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
    m_pModel = new Model("soldier.mesh");
    m_pModel->SetAnimationBlending(true);
	m_pModel->PlayAnimation("Walk", true);
    m_pModel->Scale(Ogre::Vector3(0.7f));

    m_iHeight = Terrain::GetHeight(m_tTargetTile.GetLocation().x, m_tTargetTile.GetLocation().z) - 23;
    m_pModel->SetHeightAdjustment(m_iHeight);
    m_fXOffset = (((float)(rand() % 1001) / 1000) - 0.5f) * Tile::GetDefaultSize() / 1.1;
    m_fZOffset = (((float)(rand() % 1001) / 1000) - 0.5f) * Tile::GetDefaultSize() / 1.1;

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
    m_pModel->GetSceneNode()->getChild(m_pModel->GetSceneNode()->getName() + "Box")->translate(Ogre::Vector3(-5,-7,-5));
    m_pModel->GetSceneNode()->getChild(m_pModel->GetSceneNode()->getName() + "Box")->scale(Ogre::Vector3(1,1.5,1));

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
// Special animation to perform if an enemy is killed by a fire attack
//-----------------------------------------------------------------------------
void Soldier::DieByFire()
{
    // If they were killed by a fire attack, darken their model and prepare a smoke particle
    m_fDeathTimeLeft = 2.05f; // Start particle emmission at 2.05 seconds in, stop at 0
    m_pModel->SetColour(Ogre::ColourValue(0,0,0));
    m_pParticleSystem = Ogre::Root::getSingletonPtr()->
                        getSceneManager(BaseApplication::GetSceneManagerName())->
                        createParticleSystem(m_pModel->GetSceneNode()->getName() + "Smoke", "SmokeX");
    m_pParticleSystem->setEmitting(false);
    m_pEffectNode = m_pModel->GetSceneNode()->createChildSceneNode();
    m_pEffectNode->translate(0, -12, 8);
    m_pEffectNode->attachObject(m_pParticleSystem);
}

void Soldier::PlayEscapeSound()
{
    int selection = rand() % 8;

    if(selection == 0)
    {
        SoundManager::Play3DPitchedSound("soldierEscape1.wav", m_pModel->GetLocation(), 0.05);
    }
    else if (selection == 1)
    {
        SoundManager::Play3DPitchedSound("soldierEscape2.wav", m_pModel->GetLocation(), 0.05);
    }
    else if (selection == 2)
    {
        SoundManager::Play3DPitchedSound("soldierEscape3.wav", m_pModel->GetLocation(), 0.05);
    }
    else if (selection == 3)
    {
        SoundManager::Play3DPitchedSound("soldierEscape4.wav", m_pModel->GetLocation(), 0.05);
    }
    else if (selection == 4)
    {
        SoundManager::Play3DPitchedSound("soldierEscape5.wav", m_pModel->GetLocation(), 0.05);
    }
    else if (selection == 5)
    {
        SoundManager::Play3DPitchedSound("soldierEscape6.wav", m_pModel->GetLocation(), 0.05);
    }
    else if (selection == 6)
    {
        SoundManager::Play3DPitchedSound("soldierEscape7.wav", m_pModel->GetLocation(), 0.05);
    }
    else
    {
        SoundManager::Play3DPitchedSound("soldierEscape8.wav", m_pModel->GetLocation(), 0.05);
    }

}