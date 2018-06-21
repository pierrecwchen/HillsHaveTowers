#include "ResourceManager.h"

std::vector<BaseResource*> ResourceManager::m_vResources;

//-----------------------------------------------------------------------------
// Create a new resource of a given type
//-----------------------------------------------------------------------------
BaseResource* ResourceManager::Add(Resources p_eType, Tile* p_pTile, int p_iAmount)
{
    if (p_eType == WoodResource)
        m_vResources.push_back(new Wood(p_pTile, p_iAmount));
    else if (p_eType == StoneResource)
        m_vResources.push_back(new Stone(p_pTile, p_iAmount));
    else if (p_eType == IronResource)
        m_vResources.push_back(new Iron(p_pTile, p_iAmount));

	/*****************************************************************************
	*	Add the pointer to Selectable Manager (By Pierre)
	******************************************************************************/
	m_vResources.at(m_vResources.size()-1)->SetSelectableObjType(SO_Resource);
	m_vResources.at(m_vResources.size()-1)->SetRecipientType(Hills::R_Resource);
	SelectableManager::Add(m_vResources.at(m_vResources.size()-1));
	Hills::EventManager::AddRecipient(m_vResources.at(m_vResources.size()-1), 1, Hills::GE_NULL);

	/*****************************************************************************/
    
    return m_vResources.at(m_vResources.size()-1);
}

//-----------------------------------------------------------------------------
// Remove a resource from the vector
//-----------------------------------------------------------------------------
void ResourceManager::Delete(BaseResource* p_pResource)
{
	/*****************************************************************************
	*	Remove the pointer from Selectable and Event Manager (By Pierre)
	******************************************************************************/
	SelectableManager::Remove(p_pResource);
	Hills::EventManager::RemoveRecipient(p_pResource);

	/*****************************************************************************/
    
	// Determine the resource's position in the vector
    for (int i = 0; i < m_vResources.size(); i++)
    {
        if (p_pResource == m_vResources.at(i))
        {
            m_vResources.at(i)->DestroyModel();
            m_vResources.erase(m_vResources.begin()+i);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Remove a resource at the given location in the level file
//-----------------------------------------------------------------------------
void ResourceManager::Delete(int p_iX, int p_iY)
{    
    for (int i = 0; i < m_vResources.size(); i++)
    {
        if (m_vResources.at(i)->GetLocation() == TileSet::GetTileAt(p_iX, p_iY)->GetLocation())
        {
            /*****************************************************************************
	        *	Remove the pointer from Selectable and Event Manager (By Pierre)
	        ******************************************************************************/
	        SelectableManager::Remove(m_vResources.at(i));
	        Hills::EventManager::RemoveRecipient(m_vResources.at(i));

	        /*****************************************************************************/

            m_vResources.at(i)->GetTile()->SetHasResource(false);

            if(m_vResources.at(i)->GetTile()->IsOpenPath())
            {
                TileSet::FindShortestPath();
                Hills::EventManager::AddEvent(Hills::GameEvent(Hills::GE_OpenNewPath, TileSet::GetShortestPath()));
            }

            m_vResources.at(i)->DestroyModel();
            m_vResources.erase(m_vResources.begin()+i);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Remove all resources from the vector
//-----------------------------------------------------------------------------
void ResourceManager::DeleteAll() 
{ 
	/*****************************************************************************
	*	Remove all pointers from Selectable and Event Manager (By Pierre)
	******************************************************************************/
	SelectableManager::RemoveAll(SO_Resource);
	Hills::EventManager::RemoveAllRecipients(Hills::R_Resource);

	/*****************************************************************************/

    for (int i = 0; i < m_vResources.size(); i++)
        m_vResources.at(i)->DestroyModel();
        
    m_vResources.clear();
}

//-----------------------------------------------------------------------------
// Update each resource in the vector, removing any flagged for deletion
//-----------------------------------------------------------------------------
void ResourceManager::Update(const Ogre::FrameEvent& p_feEvent)
{
    for (int i = 0; i < m_vResources.size(); i++)
    {
        if (m_vResources.at(i)->CanDelete())
        {
			/*****************************************************************************
			*	Remove the pointer from Selectable and Event Manager (By Pierre)
			******************************************************************************/
			SelectableManager::Remove(m_vResources[i]);
			Hills::EventManager::RemoveRecipient(m_vResources[i]);

			/*****************************************************************************/
            m_vResources.at(i)->DestroyModel();
            m_vResources.erase(m_vResources.begin()+i);
        }
        else
            m_vResources.at(i)->Update(p_feEvent);
    }
}