//-----------------------------------------------------------------------------
// EnemyManager.cpp                                         By: Keenan Johnston
// 
// Implementation file for EnemyManager.h
//-----------------------------------------------------------------------------

#include "EnemyManager.h"
#include "EventManager.h"

EnemyManager* EnemyManager::m_emInstance;

EnemyManager* EnemyManager::Instance()
{
    if(m_emInstance == NULL)
    {
        m_emInstance = new EnemyManager();
    }

    return m_emInstance;
}

//-----------------------------------------------------------------------------
// Create a new enemy with the default parameters for each type
//-----------------------------------------------------------------------------
BaseEnemy* EnemyManager::Add(Enemies p_eType)
{
    if (p_eType == SoldierEnemy)
        m_vEnemies.push_back(new Soldier(TileSet::GetShortestPath()));
    else if (p_eType == ScoutEnemy)
        m_vEnemies.push_back(new Scout(TileSet::GetShortestPath()));
    else if (p_eType == PriestEnemy)
        m_vEnemies.push_back(new Priest(TileSet::GetShortestPath()));
    else if (p_eType == KnightEnemy)
        m_vEnemies.push_back(new Knight(TileSet::GetShortestPath()));
    else if (p_eType == ET_DEMON)
        m_vEnemies.push_back(new Demon(TileSet::GetShortestPath()));

	/*****************************************************************************
	*	Add the pointer to Selectable and Event Manager (By Pierre)
	******************************************************************************/
	m_vEnemies.at(m_vEnemies.size()-1)->SetSelectableObjType(SO_Enemy);
	m_vEnemies.at(m_vEnemies.size()-1)->SetRecipientType(Hills::R_Enemy);
	SelectableManager::Add(m_vEnemies.at(m_vEnemies.size()-1));
	Hills::EventManager::AddRecipient(m_vEnemies.at(m_vEnemies.size()-1),
												4,
												Hills::GE_AreaAttack,
												Hills::GE_Heal,
												Hills::GE_OpenNewPath,
												Hills::GE_SlowAttack);

	/*****************************************************************************/
    
    return m_vEnemies.at(m_vEnemies.size()-1);
}

BaseEnemy* EnemyManager::Add(Enemies p_eType, float p_fXOffset, float p_fZOffset)
{
    if (p_eType == SoldierEnemy)
        m_vEnemies.push_back(new Soldier(TileSet::GetShortestPath(), p_fXOffset, p_fZOffset));
    else if (p_eType == ScoutEnemy)
        m_vEnemies.push_back(new Scout(TileSet::GetShortestPath(), p_fXOffset, p_fZOffset));
    else if (p_eType == PriestEnemy)
        m_vEnemies.push_back(new Priest(TileSet::GetShortestPath(), p_fXOffset, p_fZOffset));
    else if (p_eType == KnightEnemy)
        m_vEnemies.push_back(new Knight(TileSet::GetShortestPath(), p_fXOffset, p_fZOffset));
    else if (p_eType == ET_DEMON)
        m_vEnemies.push_back(new Demon(TileSet::GetShortestPath(), p_fXOffset, p_fZOffset));

	/*****************************************************************************
	*	Add the pointer to Selectable and Event Manager (By Pierre)
	******************************************************************************/
	m_vEnemies.at(m_vEnemies.size()-1)->SetSelectableObjType(SO_Enemy);
	m_vEnemies.at(m_vEnemies.size()-1)->SetRecipientType(Hills::R_Enemy);
	SelectableManager::Add(m_vEnemies.at(m_vEnemies.size()-1));
	Hills::EventManager::AddRecipient(m_vEnemies.at(m_vEnemies.size()-1),
												4,
												Hills::GE_AreaAttack,
												Hills::GE_Heal,
												Hills::GE_OpenNewPath,
												Hills::GE_SlowAttack,
                                                Hills::GE_DayNightChange);

	/*****************************************************************************/
    
    return m_vEnemies.at(m_vEnemies.size()-1);
}

//-----------------------------------------------------------------------------
// Remove an enemy from the vector
//-----------------------------------------------------------------------------
void EnemyManager::Delete(BaseEnemy* p_eEnemy)
{
	/*****************************************************************************
	*	Remove the pointer from Selectable and Event Manager (By Pierre)
	******************************************************************************/
	SelectableManager::Remove(p_eEnemy);
	Hills::EventManager::RemoveRecipient(p_eEnemy);

	/*****************************************************************************/
    
	// Determine the enemy's position in the vector
    for (int i = 0; i < m_vEnemies.size(); i++)
    {
        if (p_eEnemy == m_vEnemies.at(i))
        {
            SelectionCircle::Deselect(m_vEnemies.at(i)->GetSceneNode());
            m_vEnemies.at(i)->DestroyModel();
            m_vEnemies.erase(m_vEnemies.begin()+i);
            break;
        }
    }
}
//-----------------------------------------------------------------------------
// Remove all enemies from the vector
//-----------------------------------------------------------------------------
void EnemyManager::DeleteAll() 
{ 
	/*****************************************************************************
	*	Remove all pointers from Selectable and Event Manager (By Pierre)
	******************************************************************************/
	SelectableManager::RemoveAll(SO_Enemy);
	Hills::EventManager::RemoveAllRecipients(Hills::R_Enemy);

	/*****************************************************************************/

    for (int i = 0; i < m_vEnemies.size(); i++)
		delete m_vEnemies[i];
        //m_vEnemies.at(i)->DestroyModel();
        
    m_vEnemies.clear();
}
//-----------------------------------------------------------------------------
// Update all enemies and delete any dead ones that have finished playing
// their animation
//-----------------------------------------------------------------------------
void EnemyManager::Update(const Ogre::FrameEvent& p_feEvent)
{
    for (int i = 0; i < m_vEnemies.size(); i++)
    {
        if (m_vEnemies.at(i)->CanDelete())
        {
			/*****************************************************************************
			*	Remove the pointer from Selectable and Event Manager (By Pierre)
			******************************************************************************/
			SelectableManager::Remove(m_vEnemies[i]);
			Hills::EventManager::RemoveRecipient(m_vEnemies[i]);

			/*****************************************************************************/
            SelectionCircle::Deselect(m_vEnemies.at(i)->GetSceneNode());
            m_vEnemies.at(i)->DestroyModel();
            m_vEnemies.erase(m_vEnemies.begin()+i);
            i--;
        }
        else
            m_vEnemies.at(i)->Update(p_feEvent);
    }
}
//-----------------------------------------------------------------------------
// Determine if any enemies are within the range of a given tower
//-----------------------------------------------------------------------------
bool EnemyManager::EnemiesInRange(Ogre::Vector3 p_v3Loc, float p_fRange, float p_fBlindness)
{
	bool enemiesInRange = false;
	
	for (int i = 0; i < m_vEnemies.size(); i++)
	{
		Ogre::Vector3 enemyLoc = m_vEnemies.at(i)->GetLocation();

		float inRange = sqrt(pow(enemyLoc.x - p_v3Loc.x, 2) + pow(enemyLoc.z - p_v3Loc.z, 2));

        if(!m_vEnemies.at(i)->IsIdentified())
        {
            inRange /= p_fBlindness;
        }

        if (inRange <= p_fRange  && m_vEnemies.at(i)->GetState() == WalkState)
		{
			enemiesInRange = true;
			break;
		}
	}
	
	return enemiesInRange;
}
//-----------------------------------------------------------------------------
// Generate and return a list of all enemies in range of a given tower
//-----------------------------------------------------------------------------
std::vector<BaseEnemy*> EnemyManager::GetEnemiesInRange(Ogre::Vector3 p_v3Loc, 
                                                        float p_fRange, float p_fBlindness)
{
	std::vector<BaseEnemy*> enemiesInRange;
	
	for (int i = 0; i < m_vEnemies.size(); i++)
	{
		Ogre::Vector3 enemyLoc = m_vEnemies.at(i)->GetLocation();
		float inRange = sqrt(pow(enemyLoc.x - p_v3Loc.x, 2) + pow(enemyLoc.z - p_v3Loc.z, 2));
		
        if(!m_vEnemies.at(i)->IsIdentified())
        {
            inRange /= p_fBlindness;
        }

		if (inRange <= p_fRange && m_vEnemies.at(i)->GetState() == WalkState)
			enemiesInRange.push_back(m_vEnemies.at(i));
	}
	
	return enemiesInRange;
}