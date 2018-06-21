#ifndef _GAMEDATAMANAGER_
#define _GAMEDATAMANAGER_

#include "GameData.h"

class GameDataManager
{
public:
	static GameDataManager* GetSingletonPtr();
	static void Destroy();

	void Init();

	GameData* AddGameData();
	bool ChangeToGameData(const unsigned int p_uiDataIndex);
	bool ChangeToGameData(const std::string& p_strFileName);
	bool RemoveGameData(const std::string& p_strFileName, const std::string& p_strFileDir = "../GameData/");

	GameData* GetGameData(const std::string& p_strFileName);
	GameData* GetCurrentGameData()const{ return m_pCurrentData; }

	bool Save(const std::string& p_strFileDir = "../GameData/");
	bool Load(const std::string& p_strFileName, const std::string& p_strFileDir = "../GameData/");

private:
	GameDataManager();
	~GameDataManager();

	short FindGameData(const std::string& p_strFileName);

	static GameDataManager* m_pGameDataManager;

	int m_iCurrentData;

	bool m_bInit;

	GameData* m_pCurrentData;

	tinyxml2::XMLDocument* m_pXMLDocument;
	tinyxml2::XMLElement* m_pRootElement;

	std::vector<GameData*> m_vGameData;
};

#endif