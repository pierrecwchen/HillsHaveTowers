//-------------------------------------------------------------------------------------------
//	UIManager.h											Original Author: Pierre Chen
//
//	An adapter class which create bunch of essential UI for the game.
//-------------------------------------------------------------------------------------------

#ifndef _UIMANAGER_
#define _UIMANAGER_

#include <include/RendererModules/Ogre/CEGUIOgreRenderer.h>
#include "MainMenu.h"
#include "InGameHUD.h"
#include "PauseMenu.h"
#include "LoadingScreen.h"
#include "TileMenu.h"
#include "TowerMenu.h"
#include "EnemyBar.h"
#include "OptionMenu.h"
#include "LoadDataMenu.h"
#include "SaveDataMenu.h"
#include <map>

class UIManager
{
private:
	static int m_iVillagerCount;
	static int m_iVillagerStolen;

	static bool m_bResource;
	static bool m_bImageset;
	static bool m_bUIInit;

	static std::string m_strCurrentTopWindow;

	static std::map<std::string, BaseWindow*> m_mapWindows;

	static bool ReadDebugMenu(const std::string& p_strFileName, CEGUI::Window* p_pWindow);

	static void CreateRootWindow();
	static void CreateGameLogo();
	static void CreateLoadingScreen();
	static void CreateMainMenu();
	static void CreateDebugMenu(const std::string& p_strFileName);
	static void CreateInGameHUD();
	static void CreatePauseMenu();
	static void CreateTileMenu();
	static void CreateTowerMenu();
	static void CreateOptionMenu();
	static void CreateLoadDataMenu();
	static void CreateSaveDataMenu();
	
public:
	UIManager();
	~UIManager();

	static void SetupResource();
	static void SetupImageset();
	static void UIInit();

	static void Update();

	static void SwitchWindows(const std::string& p_strWindowToFront, const std::string& p_strWindowToBack = m_strCurrentTopWindow);
	
	static const std::string& GetCurrentTopWindow(){ return m_strCurrentTopWindow; }
	static CEGUI::Window* GetUIWindow(const std::string& p_strWindowName);

	static void DestroyAllUI();
};

#endif