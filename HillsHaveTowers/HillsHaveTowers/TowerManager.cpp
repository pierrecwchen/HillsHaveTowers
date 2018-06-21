//-----------------------------------------------------------------------------
// TowerManager.cpp                                         By: Keenan Johnston
// 
// Implementation file for TowerManager.h
//-----------------------------------------------------------------------------

#include "TowerManager.h"

TowerManager* TowerManager::m_tmInstance;

TowerManager* TowerManager::Instance()
{
    if(m_tmInstance == NULL)
    {
        m_tmInstance = new TowerManager();
    }

    return m_tmInstance;
}

//-----------------------------------------------------------------------------
// Create a new tower with the default parameters for each type
//-----------------------------------------------------------------------------
BaseTower* TowerManager::Add(Towers p_eType, Tile* p_pTile)
{
    if (p_eType == eArrowTower)
        m_vTowers.push_back(new ArrowTower(p_pTile, m_bIsNight));
    else if (p_eType == eCannonTower)
        m_vTowers.push_back(new CannonTower(p_pTile, m_bIsNight));
    else if (p_eType == eSniperTower)
        m_vTowers.push_back(new SniperTower(p_pTile, m_bIsNight));
    else if (p_eType == eBurstTower)
        m_vTowers.push_back(new BurstTower(p_pTile, m_bIsNight));
    else if (p_eType == eSlowTower)
        m_vTowers.push_back(new SlowTower(p_pTile, m_bIsNight));
    else if (p_eType == T_TESLA)
        m_vTowers.push_back(new TeslaTower(p_pTile, m_bIsNight));
    else if (p_eType == T_MAGIC)
        m_vTowers.push_back(new MagicTower(p_pTile, m_bIsNight));
    else if (p_eType == T_TORCH)
        m_vTowers.push_back(new TorchTower(p_pTile, m_bIsNight));

	/*****************************************************************************
	*	Add the pointer to Selectable/Event Manager (By Pierre)
	******************************************************************************/
	m_vTowers.at(m_vTowers.size()-1)->SetSelectableObjType(SO_Tower);
	m_vTowers.at(m_vTowers.size()-1)->SetRecipientType(Hills::R_Tower);
	SelectableManager::Add(m_vTowers.at(m_vTowers.size()-1));
    Hills::EventManager::AddRecipient(m_vTowers.at(m_vTowers.size()-1), 1, Hills::GE_FreezeAttack,
                                                                           Hills::GE_DayNightChange);

	/*****************************************************************************/

    return m_vTowers.at(m_vTowers.size()-1);
}
//-----------------------------------------------------------------------------
// Create a new tower with the given parameters, add it to the vector and
// pass it back
//-----------------------------------------------------------------------------
BaseTower* TowerManager::Add(Towers p_eType, float p_fRange, float p_fAtkRate, float p_fDamage, 
                              float p_fBuildTime, std::string p_sMesh, 
                              Ogre::SceneManager* p_smManager)
{
    if (p_eType == eArrowTower)
        m_vTowers.push_back(new ArrowTower(p_fRange, p_fAtkRate, p_fDamage, 
                                           p_fBuildTime, p_sMesh, p_smManager));
    else if (p_eType == eCannonTower)
        m_vTowers.push_back(new CannonTower(p_fRange, p_fAtkRate, p_fDamage, 
                                            p_fBuildTime, p_sMesh, p_smManager));
    else if (p_eType == eSniperTower)
        m_vTowers.push_back(new SniperTower(p_fRange, p_fAtkRate, p_fDamage, 
                                            p_fBuildTime, p_sMesh, p_smManager));
    else if (p_eType == eBurstTower)
        m_vTowers.push_back(new BurstTower(p_fRange, p_fAtkRate, p_fDamage, 
                                           p_fBuildTime, p_sMesh, p_smManager));
    else if (p_eType == eSlowTower)
        m_vTowers.push_back(new SlowTower(p_fRange, p_fAtkRate, p_fDamage, 
                                          p_fBuildTime, p_sMesh, p_smManager));

	/*****************************************************************************
	*	Add the pointer to Selectable/Event Manager (By Pierre)
	******************************************************************************/
	m_vTowers.at(m_vTowers.size()-1)->SetSelectableObjType(SO_Tower);
	m_vTowers.at(m_vTowers.size()-1)->SetRecipientType(Hills::R_Tower);
	SelectableManager::Add(m_vTowers.at(m_vTowers.size()-1));
	Hills::EventManager::AddRecipient(m_vTowers.at(m_vTowers.size()-1), 1, Hills::GE_FreezeAttack,
                                                                           Hills::GE_DayNightChange);

	/*****************************************************************************/
    
    return m_vTowers.at(m_vTowers.size()-1);
}
//-----------------------------------------------------------------------------
// Remove a tower from the vector
//-----------------------------------------------------------------------------
void TowerManager::Delete(BaseTower* p_pTower)
{
	/*****************************************************************************
	*	Remove the pointer from Selectable/Event Manager (By Pierre)
	******************************************************************************/
	SelectableManager::Remove(p_pTower);
	Hills::EventManager::RemoveRecipient(p_pTower);

	/*****************************************************************************/

    // Determine the tower's position in the vector
    int pos = 0;
    for (int i = 0; i < m_vTowers.size(); i++)
    {
        if (p_pTower == m_vTowers.at(i))  // will need to change
        {
            m_vTowers.at(i)->DestroyModel();
            m_vTowers.erase(m_vTowers.begin()+i);
            break;
        }
    }
}
//-----------------------------------------------------------------------------
// Remove all enemies from the vector
//-----------------------------------------------------------------------------
void TowerManager::DeleteAll()
{
	/*****************************************************************************
	*	Remove all pointers from Selectable/Event Manager (By Pierre)
	******************************************************************************/
	SelectableManager::RemoveAll(SO_Tower);
	Hills::EventManager::RemoveAllRecipients(Hills::R_Tower);

	/*****************************************************************************/

	m_pSelectedTower = 0;

    for (int i = 0; i < m_vTowers.size(); i++)
        m_vTowers.at(i)->DestroyModel();
        
    m_vTowers.clear(); 
}
//-----------------------------------------------------------------------------
// Update all towers and remove any flagged for deletion
//-----------------------------------------------------------------------------
void TowerManager::Update(const Ogre::FrameEvent& p_feEvent)
{
    for (int i = 0; i < m_vTowers.size(); i++)
    {
        if (m_vTowers.at(i)->CanDelete())
        {
			/*****************************************************************************
			*	Remove the pointer from Selectable/Event Manager (By Pierre)
			******************************************************************************/
			SelectableManager::Remove(m_vTowers[i]);
			Hills::EventManager::RemoveRecipient(m_vTowers[i]);

			/*****************************************************************************/

            m_vTowers.at(i)->DestroyModel();
            m_vTowers.erase(m_vTowers.begin()+i);
        }
        else
            m_vTowers.at(i)->Update(p_feEvent);
    }
}

void TowerManager::HandleEvent(const Hills::GameEvent& p_geGameEvent)
{
    m_bIsNight = p_geGameEvent.IsNight();
}

TowerManager::TowerManager()
    : m_bIsNight(false)
{
    Hills::EventManager::AddRecipient(this, 1, Hills::GE_DayNightChange);
}