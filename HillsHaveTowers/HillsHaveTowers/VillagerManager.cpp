//-----------------------------------------------------------------------------
// VillagerManager.cpp                                      By: Keenan Johnston
// 
// Implementation file for VillagerManager.h
//-----------------------------------------------------------------------------

#include "VillagerManager.h"
#include "SelectableManager.h"
#include "EventManager.h"

int VillagerManager::m_iVillagersStolen = 0;
std::vector<Villager*> VillagerManager::m_vVillagers;	
std::list<Tile> VillagerManager::m_lShortestPath;

bool VillagerManager::m_bVillageCreated = false;

//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
// NICK: Create a new villager at the proper location on the map
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
void VillagerManager::CreateVillage()
{
    Village::Instance();
    m_bVillageCreated = true;
}

//-----------------------------------------------------------------------------
// Add a new villager to the vector
//-----------------------------------------------------------------------------
Villager* VillagerManager::Add(Ogre::Vector3 p_v3Loc, Tile p_tStartTile, std::list<Tile> p_lPath)
{
    m_vVillagers.push_back(new Villager(p_v3Loc, p_tStartTile, p_lPath));

	/*****************************************************************************
	*	Add the pointer to Selectable and Event Manager (By Pierre)
	******************************************************************************/
	m_vVillagers.at(m_vVillagers.size()-1)->SetRecipientType(Hills::R_Villager);
	Hills::EventManager::AddRecipient(m_vVillagers.at(m_vVillagers.size()-1), 1, Hills::GE_OpenNewPath);

	/*****************************************************************************/

    return m_vVillagers.at(m_vVillagers.size()-1);
}
//-----------------------------------------------------------------------------
// Remove a villager from the vector
//-----------------------------------------------------------------------------
void VillagerManager::Delete(Villager* p_vVillager)
{
	/*****************************************************************************
	*	Remove the pointer from Selectable and Event Manager (By Pierre)
	******************************************************************************/
	Hills::EventManager::RemoveRecipient(p_vVillager);

	/*****************************************************************************/

    // Determine the villager's position in the vector
    int pos = 0;
    for (int i = 0; i < m_vVillagers.size(); i++)
    {
        if (p_vVillager == m_vVillagers.at(i))
        {
            pos = i;
            break;
        }
    }

    m_vVillagers.erase(m_vVillagers.begin()+pos);
}

//-----------------------------------------------------------------------------
// Remove all enemies from the vector
//-----------------------------------------------------------------------------
void VillagerManager::DeleteAll() 
{ 
	/*****************************************************************************
	*	Remove all pointers from Selectable and Event Manager (By Pierre)
	******************************************************************************/
	Hills::EventManager::RemoveAllRecipients(Hills::R_Villager);

	/*****************************************************************************/

    for (int i = 0; i < m_vVillagers.size(); i++)
        m_vVillagers.at(i)->DestroyModel();

    m_vVillagers.clear(); 
	m_lShortestPath.clear();
}

//-----------------------------------------------------------------------------
// Update all villagers in the vector and remove any flagged for deletion
//-----------------------------------------------------------------------------
void VillagerManager::Update(const Ogre::FrameEvent& p_feEvent)
{
    for (int i = 0; i < m_vVillagers.size(); i++)
    {
        if (m_vVillagers.at(i)->CanDelete())
        {
			/*****************************************************************************
			*	Remove the pointer from Selectable and Event Manager (By Pierre)
			******************************************************************************/
			Hills::EventManager::RemoveRecipient(m_vVillagers[i]);

			/*****************************************************************************/
            m_vVillagers.at(i)->DestroyModel();
            m_vVillagers.erase(m_vVillagers.begin()+i);
        }
        else
            m_vVillagers.at(i)->Update(p_feEvent);
    }

    //NICK~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
    //If the village was created
    if(m_bVillageCreated)
    {
        //Update the village
        Village::Instance()->Update(p_feEvent);
    }
    //~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
}

//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
// NICK: Remove villagers from the village.
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
int VillagerManager::RemoveFromVillage(int p_iVillagers)
{
    //If the village was created
    if(m_bVillageCreated)
    {
        //Remove the villagers and return how many villagers were actually taken
        return Village::Instance()->Remove(p_iVillagers);
    }
    else
    {
        //If there is no village created, just assume infinite villagers
        return p_iVillagers;
    }
}

//-----------------------------------------------------------------------------
// Check if any villagers are in range of a given enemy
//-----------------------------------------------------------------------------
bool VillagerManager::CollisionCheck(Ogre::Vector3 p_v3Loc)
{
	bool collision = false;
	
	for (int i = 0; i < m_vVillagers.size(); i++)
	{
		Ogre::Vector3 villagerLoc = m_vVillagers.at(i)->GetLocation();
		float inRange = sqrt(pow(villagerLoc.x - p_v3Loc.x, 2) + pow(villagerLoc.z - p_v3Loc.z, 2));
		
		if (inRange <= Tile::GetDefaultSize() * 0.5f)
        {
			collision = true;
            break;
        }
	}
	
	return collision;
}

//-----------------------------------------------------------------------------
// Return the first instance of a collided villager with an enemy
//-----------------------------------------------------------------------------
Villager* VillagerManager::GetCollidedVillager(Ogre::Vector3 p_v3Loc)
{
    Villager* villager;
	
	for (int i = 0; i < m_vVillagers.size(); i++)
	{
		Ogre::Vector3 villagerLoc = m_vVillagers.at(i)->GetLocation();
		float inRange = sqrt(pow(villagerLoc.x - p_v3Loc.x, 2) + pow(villagerLoc.z - p_v3Loc.z, 2));
		
		if (inRange <= 30.0f)
			villager = m_vVillagers.at(i);
	}
	
	return villager;
}