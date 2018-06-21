//-----------------------------------------------------------------------------
// Wood.cpp                                                 By: Keenan Johnston
// 
// Implementation file for Wood.h
//-----------------------------------------------------------------------------

#include "Wood.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Wood::Wood(Tile* p_pTile, int p_iAmount)
{
	// Set the size of the resource
	m_fAmount = p_iAmount;

    m_eType = RT_Wood;
    m_fTimePerHarvest = 2.0f;
    m_pTile = p_pTile;

	// Create the visual representation of the resource
    m_pModel = new Model("bark.mesh");
    m_pModel->Translate(m_pTile->GetLocation());
    m_fScale = 0.75f + (float(rand() % 101)/100.0f*0.25f);
    m_pModel->Scale(Ogre::Vector3(4*m_fScale));
    int rotation = rand() % 360;
    m_pModel->Rotate(rotation);

    m_pModel2 = new Model("leaves.mesh");
    m_pModel2->Translate(Ogre::Vector3(m_pTile->GetLocation().x,
                                       m_pTile->GetLocation().y + 5.2,
                                       m_pTile->GetLocation().z));
    m_pModel2->Scale(Ogre::Vector3(2*m_fScale));
    m_pModel2->Rotate(rotation);

    m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
    m_pBoundingBox->attachObject(BoundingBox::GenerateBoundingBox(m_pModel->GetSceneNode()->getName(), BT_RESOURCE));
    m_pBoundingBox->setVisible(false);
    m_pBoundingBox->yaw(-Ogre::Degree(m_pModel->GetRotation()));
    m_pBoundingBox->scale(Ogre::Vector3(0.18f,0.48f,0.18f));
}

//-----------------------------------------------------------------------------
// Clean up OGRE-related data here.  Overidden because the tree is made of two
// models, not a single one.
//-----------------------------------------------------------------------------
void Wood::DestroyModel()
{
	BaseResource::DestroyModel();
    m_pModel2->CleanUp();
	delete m_pModel2;
}

//-----------------------------------------------------------------------------
// Harvest the resource, giving one wood to the player and flagging for
// deletion if it is empty
//-----------------------------------------------------------------------------
void Wood::Harvest()
{
	m_fAmount--;
    m_pModel->ScaleTo(Ogre::Vector3(m_fScale*4*(1.0f-(5.0f-m_fAmount)/5.0f)));  // shrink the tree as it gets harvested (temp)
    m_pModel2->ScaleTo(Ogre::Vector3(m_fScale*2*(1.0f-(5.0f-m_fAmount)/5.0f)));  // shrink the tree as it gets harvested (temp)

    MainCurrency::AddWood(1);

    // Node has been fully harvested
	if (m_fAmount == 0)
	{
		m_bIsHarvestable = false;
        m_eState = eDepletedState;
        m_fTimeUntilDeath = 0.0f; // temp
        SelectionCircle::Deselect(m_pModel->GetSceneNode());

		/*****************************************************************************
		*	Cannot be selected and cannot recieve events in depleted state. (By Pierre)
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
}