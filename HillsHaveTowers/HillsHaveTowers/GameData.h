#ifndef _GAMEDATA_
#define _GAMEDATA_

#include "Player.h"
#include "tinyxml2.h"
#include <string>
#include <vector>

struct TowerInfo
{
	Towers mTowerType;
	int m_iX, m_iY, m_iLevel;

	TowerInfo(): mTowerType(eArrowTower), m_iX(0), m_iY(0), m_iLevel(1){}
	TowerInfo(Towers pTowerType, int p_iX, int p_iY, int p_iLevel): mTowerType(pTowerType), m_iX(p_iX), m_iY(p_iY), m_iLevel(p_iLevel){}
	~TowerInfo(){}
};

struct ResourceInfo
{
	Resources mResourceType;
	int m_iX, m_iY, m_fAmount;

	ResourceInfo(): mResourceType(WoodResource), m_iX(0), m_iY(0), m_fAmount(0){}
	ResourceInfo(Resources pResourceType, int p_iX, int p_iY, int p_iAmount): mResourceType(pResourceType), m_iX(p_iX), m_iY(p_iY), m_fAmount(p_iAmount){}
	~ResourceInfo(){}
};

class GameData
{
public:
	
	friend class GameDataManager;

	std::string GetFileName()const { return m_strFileName; }

	void SetLevel(int p_iLevel){ m_iLevel = p_iLevel; }
	int GetLevel()const{ return m_iLevel; }

	void SetWave(int p_iWave){ m_iWave = p_iWave; }
	int GetWave()const{ return m_iWave; }

	void SetCurrentWave(int p_iCurrentWave){ m_iCurrentWave = p_iCurrentWave; }
	int GetCurrentWave()const{ return m_iCurrentWave; }

	void SetVillager(int p_iVllager){ m_iVillager = p_iVllager; }
	int GetVillager()const{ return m_iVillager; }

	void SetVillagerStolen(int p_iVllagerStolen){ m_iVllagerStolen = p_iVllagerStolen; }
	int GetVillagerStolen()const{ return m_iVllagerStolen; }

	void SetCoin(int p_iCoin){ m_iCoin = p_iCoin; }
	int GetCoin()const { return m_iCoin; }

	void SetWood(int p_iWood){ m_iWood = p_iWood; }
	int GetWood()const { return m_iWood; }

	void SetStone(int p_iStone){ m_iStone = p_iStone; }
	int GetStone()const { return  m_iStone; }

	void SetIron(int p_iIron){ m_iIron = p_iIron; }
	int GetIron()const { return m_iIron; }

	void SetPlayerLoc(Ogre::Vector3& p_vPlayerLoc){  m_vPlayerLoc = p_vPlayerLoc; }
	Ogre::Vector3 GetPlayerLoc()const{ return m_vPlayerLoc; }

	void AddTowerInfo(Towers pTowerType, int p_iX, int p_iY, int p_iLevel);
	void AddTowerInfo(TowerInfo* p_pTowerInfo);
	void AddTowerInfo(std::vector<TowerInfo*> p_vTowers);

	void AddResourceInfo(Resources pResourceType, int p_iX, int p_iY, int p_iAmount);
	void AddResourceInfo(ResourceInfo* p_pResourceInfo);
	void AddResourceInfo(std::vector<ResourceInfo*> p_vResources);

	void RemoveTowerInfo(int p_iX, int p_iY);
	void RemoveTowerInfo(TowerInfo* p_pTowerInfo);
	void RemoveAllTowerInfo();

	void RemoveResourceInfo(int p_iX, int p_iY);
	void RemoveResourceInfo(ResourceInfo* p_pResourceInfo);
	void RemoveAllResourceInfo();

	std::vector<TowerInfo*> GetTowerInfo()const{ return m_vTowers; }
	std::vector<ResourceInfo*> GetResourceInfo()const{ return m_vResources; }


private:
	GameData();
	GameData(const std::string& p_strFileName);
	~GameData();

	static unsigned int s_uiGameDataNum;

	short FindTower(int p_iX, int p_iY);
	short FindResource(int p_iX, int p_iY);
	
	int m_iLevel;
	int m_iWave;
	int m_iCurrentWave;
	int m_iVillager;
	int m_iVllagerStolen;
	int m_iCoin;
	int m_iWood;
	int m_iStone;
	int m_iIron;

	std::string m_strFileName;

	Ogre::Vector3 m_vPlayerLoc;

	std::vector<TowerInfo*> m_vTowers;
	std::vector<ResourceInfo*> m_vResources;
};

#endif