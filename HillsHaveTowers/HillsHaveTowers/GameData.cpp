#include "GameData.h"

unsigned int GameData::s_uiGameDataNum = 0;

GameData::GameData():
	m_iLevel(0),
	m_iWave(0),
	m_iCurrentWave(0),
	m_iVillager(0),
	m_iVllagerStolen(0),
	m_iCoin(0),
	m_iWood(0),
	m_iStone(0),
	m_iIron(0),
	m_strFileName("GameData" + static_cast<std::ostringstream*>( &(std::ostringstream() << ++s_uiGameDataNum) )->str()),
	m_vPlayerLoc(0, 0, 0),
	m_vTowers(),
	m_vResources()
{
}

GameData::GameData(const std::string& p_strFileName):
	m_iLevel(0),
	m_iWave(0),
	m_iCurrentWave(0),
	m_iVillager(0),
	m_iVllagerStolen(0),
	m_iCoin(0),
	m_iWood(0),
	m_iStone(0),
	m_iIron(0),
	m_strFileName(p_strFileName),
	m_vPlayerLoc(0, 0, 0),
	m_vTowers(),
	m_vResources()
{
	s_uiGameDataNum++;
}

GameData::~GameData()
{
	std::vector<TowerInfo*>::iterator towerIT = m_vTowers.begin();
	std::vector<ResourceInfo*>::iterator resourceIT = m_vResources.begin();

	for(; towerIT != m_vTowers.end(); ++towerIT)
		delete *towerIT;

	for(; resourceIT != m_vResources.end(); ++resourceIT)
		delete *resourceIT;

	m_vTowers.clear();
	m_vResources.clear();
}

short GameData::FindTower(int p_iX, int p_iY)
{
	for(short i = 0; i < m_vTowers.size(); i++)
		if(m_vTowers[i]->m_iX == p_iX && m_vTowers[i]->m_iY == p_iY)
			return i;

	return -1;
}

short GameData::FindResource(int p_iX, int p_iY)
{
	for(short i = 0; i < m_vResources.size(); i++)
		if(m_vResources[i]->m_iX == p_iX && m_vResources[i]->m_iY == p_iY)
			return i;

	return -1;
}

void GameData::AddTowerInfo(Towers pTowerType, int p_iX, int p_iY, int p_iLevel)
{
	short tower = FindTower(p_iX, p_iY);

	(tower == -1)?
		m_vTowers.push_back(new TowerInfo(pTowerType, p_iX, p_iY, p_iLevel)):
		m_vTowers[tower]->mTowerType = pTowerType;
}

void GameData::AddTowerInfo(TowerInfo* p_pTowerInfo)
{
	short tower = FindTower(p_pTowerInfo->m_iX, p_pTowerInfo->m_iY);

	(tower == -1)?
		m_vTowers.push_back(p_pTowerInfo):
		m_vTowers[tower]->mTowerType = p_pTowerInfo->mTowerType;
}

void GameData::AddTowerInfo(std::vector<TowerInfo*> p_vTowers)
{
	RemoveAllTowerInfo();
	m_vTowers = p_vTowers;
}

void GameData::AddResourceInfo(Resources pResourceType, int p_iX, int p_iY, int p_iAmount)
{
	short resource = FindResource(p_iX, p_iY);

	if(resource == -1)
		m_vResources.push_back(new ResourceInfo(pResourceType, p_iX, p_iY, p_iAmount));
	else
	{
		m_vResources[resource]->mResourceType = pResourceType;
		m_vResources[resource]->m_fAmount = p_iAmount;
	}
}

void GameData::AddResourceInfo(ResourceInfo* p_pResourceInfo)
{
	short resource = FindResource(p_pResourceInfo->m_iX, p_pResourceInfo->m_iY);

	if(resource == -1)
		m_vResources.push_back(p_pResourceInfo);
	else
	{
		m_vResources[resource]->mResourceType = p_pResourceInfo->mResourceType;
		m_vResources[resource]->m_fAmount = p_pResourceInfo->m_fAmount;
	}
}

void GameData::AddResourceInfo(std::vector<ResourceInfo*> p_vResources)
{
	RemoveAllResourceInfo();
	m_vResources = p_vResources;
}

void GameData::RemoveTowerInfo(int p_iX, int p_iY)
{
	short tower = FindTower(p_iX, p_iY);

	if(tower != -1)
	{
		delete m_vTowers[tower];
		m_vTowers.erase(m_vTowers.begin()+tower);
	}
}

void GameData::RemoveTowerInfo(TowerInfo* p_pTowerInfo)
{
	short tower = FindTower(p_pTowerInfo->m_iX, p_pTowerInfo->m_iY);

	if(tower != -1)
	{
		delete m_vTowers[tower];
		m_vTowers.erase(m_vTowers.begin()+tower);
	}
}

void GameData::RemoveAllTowerInfo()
{
	std::vector<TowerInfo*>::iterator towerIT = m_vTowers.begin();

	for(; towerIT != m_vTowers.end(); ++towerIT)

	m_vTowers.clear();
}

void GameData::RemoveResourceInfo(int p_iX, int p_iY)
{
	short resource = FindResource(p_iX, p_iY);

	if(resource != -1)
	{
		delete m_vResources[resource];
		m_vResources.erase(m_vResources.begin()+resource);
	}
}

void GameData::RemoveResourceInfo(ResourceInfo* p_pResourceInfo)
{
	short resource = FindResource(p_pResourceInfo->m_iX, p_pResourceInfo->m_iY);

	if(resource != -1)
	{
		delete m_vResources[resource];
		m_vResources.erase(m_vResources.begin()+resource);
	}
}

void GameData::RemoveAllResourceInfo()
{
	std::vector<ResourceInfo*>::iterator resourceIT = m_vResources.begin();

	for(; resourceIT != m_vResources.end(); ++resourceIT)
		delete *resourceIT;

	m_vResources.clear();
}