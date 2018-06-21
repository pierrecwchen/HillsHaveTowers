//-----------------------------------------------------------------------------
// Iron.cpp                                                 By: Keenan Johnston
// 
// Implementation file for Iron.h
//-----------------------------------------------------------------------------

#include "Iron.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Iron::Iron(Tile* p_pTile, int p_iAmount)
{
	// Set the size of the resource
	m_fAmount = p_iAmount;

    m_eType = RT_Iron;
    m_fTimePerHarvest = 6.0f;
    m_pTile = p_pTile;

    m_fCircleSize = 13.0f;

	// Create the visual representation of the resource
    m_pModel = new Model("Stone.mesh");
    m_pModel->SetQueryFlag(QF_RESOURCES);
    m_pModel->Translate(m_pTile->GetLocation());
    m_fScale = 1.0f + (float(rand() % 101)/100.0f*0.25f);
    m_pModel->Scale(Ogre::Vector3(2*m_fScale));
    int rotation = rand() % 360;
    m_pModel->Rotate(rotation);
    m_pModel->GetEntity()->setMaterialName("Iron");

    m_pBoundingBox = m_pModel->GetSceneNode()->createChildSceneNode(m_pModel->GetSceneNode()->getName() + "Box");
    m_pBoundingBox->attachObject(BoundingBox::GenerateBoundingBox(m_pModel->GetSceneNode()->getName(), BT_RESOURCE));
    m_pBoundingBox->setVisible(false);
    m_pBoundingBox->yaw(-Ogre::Degree(m_pModel->GetRotation()));
    m_pBoundingBox->scale(Ogre::Vector3(0.5,0.25,0.5));
}

//-----------------------------------------------------------------------------
// Harvest the resource, giving one iron to the player and flagging for
// deletion if it is empty
//-----------------------------------------------------------------------------
void Iron::Harvest()
{
	m_fAmount--;
    m_pModel->ScaleTo(Ogre::Vector3(m_fScale*2*(1.0f-(5.0f-m_fAmount)/5.0f)));  // shrink the iron as it gets harvested (temp)

	MainCurrency::AddIron(1);

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