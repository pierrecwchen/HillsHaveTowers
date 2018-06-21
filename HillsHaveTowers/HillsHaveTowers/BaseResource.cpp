//-----------------------------------------------------------------------------
// BaseResource.cpp                                         By: Keenan Johnston
// 
// 
//-----------------------------------------------------------------------------

#include "BaseResource.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
BaseResource::BaseResource()
{
    m_bIsHarvestable = true;
    m_bCanDelete = false;
    m_eState = eIdleState;
    m_fCircleSize = 10.0f;
    m_fCircleOffset = 0.0f;
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
BaseResource::~BaseResource()
{

}

//-----------------------------------------------------------------------------
// Harvest the resource, giving one unit to the player and flagging for
// deletion if it is empty
//-----------------------------------------------------------------------------
void BaseResource::Harvest()
{
	m_fAmount--;
    m_pModel->ScaleTo(Ogre::Vector3(m_fAmount*7));  // shrink resource as it gets harvested (temp)

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

//-----------------------------------------------------------------------------
// Destroy OGRE-related data here
//-----------------------------------------------------------------------------
void BaseResource::DestroyModel()
{
	m_pTile = 0;
    m_pModel->CleanUp();
	delete m_pModel;
}

//-----------------------------------------------------------------------------
// Handle selection from the selection circle
//-----------------------------------------------------------------------------
void BaseResource::Select()
{
    SelectionCircle::LockTo(m_pModel->GetSceneNode(), m_fCircleSize, m_fCircleOffset);
}

/**************************************************************************************
*	Handle the events. (Added by Pierre)
***************************************************************************************/
void BaseResource::HandleEvent(const Hills::GameEvent& p_geGameEvent)
{

}

/**************************************************************************************
*	Show the bounding box of the entity. (Added by Pierre)
***************************************************************************************/
void BaseResource::ShowObject(bool p_bIsShow)
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

//-----------------------------------------------------------------------------
// Update this resource, if it has been depleted, open the path and flag it
// for deletion
//-----------------------------------------------------------------------------
void BaseResource::Update(const Ogre::FrameEvent& p_feEvent)
{
    // If the resource has been fully harvested, perform a small
    // animation to make it dissapear
    if (m_eState == eDepletedState)
    {
        m_fTimeUntilDeath -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

        if (m_fTimeUntilDeath <= 0)
        {
            m_pTile->SetHasResource(false);
            
            //NICK~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
            //If the resource was on the path
            if(m_pTile->IsOpenPath())
            {
                //Find the shortest path and then broadcast it to all listening members
                TileSet::FindShortestPath();
                Hills::EventManager::AddEvent(Hills::GameEvent(Hills::GE_OpenNewPath, TileSet::GetShortestPath()));
            }
            //~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*

            m_bCanDelete = true;
        }
    }
}