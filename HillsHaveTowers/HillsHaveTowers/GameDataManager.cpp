#include "GameDataManager.h"
#include <cstdio>
#include "TowerManager.h"
#include "ResourceManager.h"
#include "VillagerManager.h"
#include "LoadDataMenu.h"
#include "SaveDataMenu.h"

GameDataManager* GameDataManager::m_pGameDataManager = 0;

GameDataManager::GameDataManager(): m_bInit(false), m_iCurrentData(0), m_pCurrentData(0)
{
}

GameDataManager::~GameDataManager()
{
	m_pXMLDocument->DeleteChildren();

	if(m_pXMLDocument)
		delete m_pXMLDocument;

	std::vector<GameData*>::iterator gameDataIt = m_vGameData.begin();

	for(; gameDataIt != m_vGameData.end(); gameDataIt++)
	{
		delete *gameDataIt;
	}

	m_vGameData.clear();

	m_pCurrentData = 0;
}

GameDataManager* GameDataManager::GetSingletonPtr()
{
	if(!m_pGameDataManager)
		m_pGameDataManager = new GameDataManager();

	return m_pGameDataManager;
}

void GameDataManager::Destroy()
{
	if(m_pGameDataManager)
	{
		delete m_pGameDataManager;
		m_pGameDataManager = 0;
	}
}

void GameDataManager::Init()
{

	if(m_bInit)
		return;

	/*Load("GameData1");
	SaveDataMenu::GetSingletonPtr()->AddNewListItem(m_pCurrentData->GetFileName());
	Load("GameData2");
	SaveDataMenu::GetSingletonPtr()->AddNewListItem(m_pCurrentData->GetFileName());
	Load("GameData3");
	SaveDataMenu::GetSingletonPtr()->AddNewListItem(m_pCurrentData->GetFileName());*/

	m_pXMLDocument = new tinyxml2::XMLDocument("GameDataList");
	tinyxml2::XMLError loadDoc = m_pXMLDocument->LoadFile("../GameData/GameDataList.xml");

	if(loadDoc == tinyxml2::XML_SUCCESS)
		m_pRootElement = m_pXMLDocument->FirstChildElement("GameDataList");

	tinyxml2::XMLElement* element = m_pRootElement->FirstChildElement();

	while(element)
	{
		Load(element->Value());
		SaveDataMenu::GetSingletonPtr()->AddNewListItem(m_pCurrentData->GetFileName());
		element = element->NextSiblingElement();
	}

	AddGameData();

	m_bInit = true;
}

short GameDataManager::FindGameData(const std::string& p_strFileName)
{
	for(short i = 0; i < m_vGameData.size(); i++)
		if(m_vGameData[i]->m_strFileName == p_strFileName)
			return i;

	return -1;
}

GameData* GameDataManager::AddGameData()
{
	m_vGameData.push_back(new GameData());

	m_iCurrentData = m_vGameData.size()-1;
	m_pCurrentData = m_vGameData[m_iCurrentData];

	return m_pCurrentData;
}

bool GameDataManager::ChangeToGameData(const unsigned int p_uiDataIndex)
{
	if(p_uiDataIndex >= m_vGameData.size())
	{
		printf("Wrong index passed into ChangeToGameData method");
		return false;
	}

	m_vGameData[p_uiDataIndex]->SetPlayerLoc(m_pCurrentData->GetPlayerLoc());
	m_vGameData[p_uiDataIndex]->SetCurrentWave(m_pCurrentData->GetCurrentWave());

	m_pCurrentData = m_vGameData[p_uiDataIndex];

	return true;
}

bool GameDataManager::ChangeToGameData(const std::string& p_strFileName)
{
	short gameData = FindGameData(p_strFileName);

	if(gameData == -1)
		return false;

	m_vGameData[gameData]->SetPlayerLoc(m_pCurrentData->GetPlayerLoc());
	m_vGameData[gameData]->SetCurrentWave(m_pCurrentData->GetCurrentWave());

	m_pCurrentData = m_vGameData[gameData];

	return true;
}

bool GameDataManager::RemoveGameData(const std::string& p_strFileName, const std::string& p_strFileDir)
{
	short gameData = FindGameData(p_strFileName);

	if(gameData != -1)
	{
		if(m_pCurrentData == m_vGameData[gameData])
			m_pCurrentData = 0;

		delete m_vGameData[gameData];
		m_vGameData.erase(m_vGameData.begin()+gameData);

		if(std::remove((p_strFileDir+p_strFileName+".xml").c_str()) == 0)
			return true;
	}

	return false;
}

GameData* GameDataManager::GetGameData(const std::string& p_strFileName)
{
	short gameData = FindGameData(p_strFileName);

	if(gameData == -1)
		return 0;

	return m_vGameData[gameData];
}

bool GameDataManager::Save(const std::string& p_strFileDir)
{
	if(!m_pRootElement->FirstChildElement(m_pCurrentData->GetFileName().c_str()))
	{
		tinyxml2::XMLElement* newElement = m_pXMLDocument->NewElement(m_pCurrentData->GetFileName().c_str());
		m_pRootElement->LinkEndChild(newElement);
		std::remove("../GameData/GameDataList.xml");
		m_pXMLDocument->SaveFile("../GameData/GameDataList.xml");
	}

	std::remove((p_strFileDir + m_pCurrentData->m_strFileName + ".xml").c_str());

	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument(m_pCurrentData->m_strFileName.c_str());

	tinyxml2::XMLDeclaration* docDeclaration = doc->NewDeclaration("xml version=\"1.0\" encoding=\"utf-8\"");

	tinyxml2::XMLElement* root = doc->NewElement(m_pCurrentData->m_strFileName.c_str());

	tinyxml2::XMLElement* level = doc->NewElement("Level");
	level->SetAttribute("Level", 1);

	tinyxml2::XMLElement* wave = doc->NewElement("Wave");
	wave->SetAttribute("Wave", 1);
	wave->SetAttribute("CurrentWave", m_pCurrentData->GetCurrentWave());

	tinyxml2::XMLElement* villager = doc->NewElement("Villager");
	m_pCurrentData->SetVillager(VillagerManager::GetVillagerCount());
	m_pCurrentData->SetVillagerStolen(VillagerManager::GetNumVillagersStolen());
	villager->SetAttribute("Villager", 20);
	villager->SetAttribute("Stolen", 0);

	tinyxml2::XMLElement* currency = doc->NewElement("Currency");
	m_pCurrentData->SetCoin(MainCurrency::GetGold());
	m_pCurrentData->SetWood(MainCurrency::GetWood());
	m_pCurrentData->SetStone(MainCurrency::GetStone());
	m_pCurrentData->SetIron(MainCurrency::GetIron());
	currency->SetAttribute("Coin", m_pCurrentData->GetCoin());
	currency->SetAttribute("Wood", m_pCurrentData->GetWood());
	currency->SetAttribute("Stone", m_pCurrentData->GetStone());
	currency->SetAttribute("Iron", m_pCurrentData->GetIron());

	tinyxml2::XMLElement* player = doc->NewElement("Player");
	player->SetAttribute("XPosition", m_pCurrentData->GetPlayerLoc().x);
	player->SetAttribute("YPosition", m_pCurrentData->GetPlayerLoc().y);
	player->SetAttribute("ZPosition", m_pCurrentData->GetPlayerLoc().z);

	tinyxml2::XMLElement* towers = doc->NewElement("Towers");
	tinyxml2::XMLElement* tower;

	m_pCurrentData->m_vTowers.clear();
	
	std::vector<BaseTower*> towerVec(TowerManager::Instance()->GetAll());

	for(int i = 0; i < towerVec.size(); ++i)
	{
		if(dynamic_cast<ArrowTower*>(towerVec[i]))
			m_pCurrentData->AddTowerInfo(eArrowTower, towerVec[i]->GetTile()->GetGridX(), towerVec[i]->GetTile()->GetGridY(), towerVec[i]->GetLevel());
		else if(dynamic_cast<CannonTower*>(towerVec[i]))
			m_pCurrentData->AddTowerInfo(eCannonTower, towerVec[i]->GetTile()->GetGridX(), towerVec[i]->GetTile()->GetGridY(), towerVec[i]->GetLevel());
		else if(dynamic_cast<SniperTower*>(towerVec[i]))
			m_pCurrentData->AddTowerInfo(eSniperTower, towerVec[i]->GetTile()->GetGridX(), towerVec[i]->GetTile()->GetGridY(), towerVec[i]->GetLevel());
		else if(dynamic_cast<BurstTower*>(towerVec[i]))
			m_pCurrentData->AddTowerInfo(eBurstTower, towerVec[i]->GetTile()->GetGridX(), towerVec[i]->GetTile()->GetGridY(), towerVec[i]->GetLevel());
		else if(dynamic_cast<SlowTower*>(towerVec[i]))
			m_pCurrentData->AddTowerInfo(eSlowTower, towerVec[i]->GetTile()->GetGridX(), towerVec[i]->GetTile()->GetGridY(), towerVec[i]->GetLevel());
		else if(dynamic_cast<TeslaTower*>(towerVec[i]))
			m_pCurrentData->AddTowerInfo(T_TESLA, towerVec[i]->GetTile()->GetGridX(), towerVec[i]->GetTile()->GetGridY(), towerVec[i]->GetLevel());
	}

	for(int i = 0; i < m_pCurrentData->m_vTowers.size(); ++i)
	{
		tower = doc->NewElement("Tower");
		tower->SetAttribute("TowerType", static_cast<int>(m_pCurrentData->m_vTowers[i]->mTowerType));
		tower->SetAttribute("XPosition", m_pCurrentData->m_vTowers[i]->m_iX);
		tower->SetAttribute("YPosition", m_pCurrentData->m_vTowers[i]->m_iY);
		tower->SetAttribute("Level", m_pCurrentData->m_vTowers[i]->m_iLevel);

		towers->LinkEndChild(tower);
	}

	tinyxml2::XMLElement* resources = doc->NewElement("Resources");
	tinyxml2::XMLElement* resource;

	m_pCurrentData->m_vResources.clear();

	std::vector<BaseResource*> resourceVec(ResourceManager::GetAll());

	for(int i = 0; i < resourceVec.size(); ++i)
	{
		if(dynamic_cast<Wood*>(resourceVec[i]))
			m_pCurrentData->AddResourceInfo(WoodResource, resourceVec[i]->GetTile()->GetGridX(), resourceVec[i]->GetTile()->GetGridY(), resourceVec[i]->GetAmount());
		else if(dynamic_cast<Stone*>(resourceVec[i]))
			m_pCurrentData->AddResourceInfo(StoneResource, resourceVec[i]->GetTile()->GetGridX(), resourceVec[i]->GetTile()->GetGridY(), resourceVec[i]->GetAmount());
		else if(dynamic_cast<Iron*>(resourceVec[i]))
			m_pCurrentData->AddResourceInfo(IronResource, resourceVec[i]->GetTile()->GetGridX(), resourceVec[i]->GetTile()->GetGridY(), resourceVec[i]->GetAmount());
	}

	for(int i = 0; i < m_pCurrentData->m_vResources.size(); ++i)
	{
		resource = doc->NewElement("Resource");
		resource->SetAttribute("ResourceType", static_cast<int>(m_pCurrentData->m_vResources[i]->mResourceType));
		resource->SetAttribute("XPosition", m_pCurrentData->m_vResources[i]->m_iX);
		resource->SetAttribute("YPosition", m_pCurrentData->m_vResources[i]->m_iY);
		resource->SetAttribute("Amount", m_pCurrentData->m_vResources[i]->m_fAmount);

		resources->LinkEndChild(resource);
	}

	doc->LinkEndChild(docDeclaration);

	root->LinkEndChild(level);
	root->LinkEndChild(wave);
	root->LinkEndChild(villager);
	root->LinkEndChild(currency);
	root->LinkEndChild(player);
	root->LinkEndChild(towers);
	root->LinkEndChild(resources);

	doc->LinkEndChild(root);

	doc->SaveFile((p_strFileDir + m_pCurrentData->m_strFileName + ".xml").c_str());

	doc->DeleteChildren();

	delete doc;

	return true;
}

bool GameDataManager::Load(const std::string& p_strFileName, const std::string& p_strFileDir)
{
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument(p_strFileName.c_str());
	tinyxml2::XMLError loadDoc = doc->LoadFile((p_strFileDir + p_strFileName + ".xml").c_str());

	if(loadDoc == tinyxml2::XML_ERROR_FILE_NOT_FOUND)
	{
		if(doc)
			delete doc;
		return false;
	}

	else if(loadDoc == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLElement* root = doc->FirstChildElement();

		short found = FindGameData(p_strFileName);
		if(found == -1)
			AddGameData();
		else
			m_pCurrentData = m_vGameData[found];

		tinyxml2::XMLElement* level = root->FirstChildElement("Level");
		m_pCurrentData->SetLevel(level->IntAttribute("Level"));

		tinyxml2::XMLElement* wave = root->FirstChildElement("Wave");
		m_pCurrentData->SetWave(wave->IntAttribute("Wave"));
		m_pCurrentData->SetCurrentWave(wave->IntAttribute("CurrentWave"));

		tinyxml2::XMLElement* villager = root->FirstChildElement("Villager");
		m_pCurrentData->SetVillager(villager->IntAttribute("Villager"));
		m_pCurrentData->SetVillagerStolen(villager->IntAttribute("Stolen"));

		tinyxml2::XMLElement* currency = root->FirstChildElement("Currency");
		m_pCurrentData->SetCoin(currency->IntAttribute("Coin"));
		m_pCurrentData->SetWood(currency->IntAttribute("Wood"));
		m_pCurrentData->SetStone(currency->IntAttribute("Stone"));
		m_pCurrentData->SetIron(currency->IntAttribute("Iron"));

		tinyxml2::XMLElement* player = root->FirstChildElement("Player");
		m_pCurrentData->SetPlayerLoc(Ogre::Vector3(player->FloatAttribute("XPosition"), player->FloatAttribute("YPosition"), player->FloatAttribute("ZPosition")));

		m_pCurrentData->RemoveAllTowerInfo();
		tinyxml2::XMLElement* towers = root->FirstChildElement("Towers");
		tinyxml2::XMLElement* tower = towers->FirstChildElement();

		while(tower)
		{
			Towers towerType;
			switch(tower->IntAttribute("TowerType"))
			{
				case 0:
					towerType = eArrowTower;
					break;
				case 1:
					towerType = eCannonTower;
					break;
				case 2:
					towerType = eSniperTower;
					break;
				case 3:
					towerType = eBurstTower;
					break;
				case 4:
					towerType = eSlowTower;
					break;
				case 5:
					towerType = T_TESLA;
					break;
				default:
					towerType = eArrowTower;
					break;
			}

			m_pCurrentData->AddTowerInfo(towerType,
										tower->IntAttribute("XPosition"),
										tower->IntAttribute("YPosition"),
										tower->IntAttribute("Level"));

			tower = tower->NextSiblingElement();
		}

		m_pCurrentData->RemoveAllResourceInfo();
		tinyxml2::XMLElement* resources = root->FirstChildElement("Resources");
		tinyxml2::XMLElement* resource = resources->FirstChildElement();

		while(resource)
		{
			Resources resourceType;
			switch(resource->IntAttribute("ResourceType"))
			{
				case 0:
					resourceType = WoodResource;
					break;
				case 1:
					resourceType = StoneResource;
					break;
				case 2:
					resourceType = IronResource;
					break;
				default:
					resourceType = WoodResource;
					break;
			}

			m_pCurrentData->AddResourceInfo(resourceType,
										resource->IntAttribute("XPosition"),
										resource->IntAttribute("YPosition"),
										resource->IntAttribute("Amount"));

			resource = resource->NextSiblingElement();
		}

	}

	doc->DeleteChildren();
	if(doc)
		delete doc;
	
	return true;
}