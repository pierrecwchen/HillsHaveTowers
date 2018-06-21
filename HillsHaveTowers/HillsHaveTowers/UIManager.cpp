//-------------------------------------------------------------------------------------------
//	UIManager.cpp											Original Author: Pierre Chen
//
//	Please see notes from UIManager.h
//-------------------------------------------------------------------------------------------
#include "UIManager.h"
#include "VillagerManager.h"

int UIManager::m_iVillagerCount = 20;
int UIManager::m_iVillagerStolen = 0;
bool UIManager::m_bResource = false;
bool UIManager::m_bImageset = false;
bool UIManager::m_bUIInit = false;
std::string UIManager::m_strCurrentTopWindow = "";
std::map<std::string, BaseWindow*> UIManager::m_mapWindows;

//-------------------------------------------------------------------------------------------
//	Default constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
UIManager::UIManager()
{
}

//-------------------------------------------------------------------------------------------
//	Default destructor. (By Pierre)
//-------------------------------------------------------------------------------------------
UIManager::~UIManager()
{
}

//-------------------------------------------------------------------------------------------
//	Set up UI resource. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::SetupResource()
{
	if(m_bResource)
		return;

	// Set default resource group
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

	// Create schemes and default mouse cursor
	CEGUI::SchemeManager::getSingleton().create("WindowsLook.scheme");
	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	CEGUI::System::getSingleton().setDefaultMouseCursor("WindowsLook", "MouseArrow");

	// Create fonts
	CEGUI::FontManager::getSingleton().create("DejaVuSans-10.font", "Fonts");
	CEGUI::FontManager::getSingleton().create("DejaVuSans-5.font", "Fonts");
	CEGUI::FontManager::getSingleton().create("fkp-16.font", "Fonts");
	CEGUI::System::getSingleton().setDefaultFont("fkp-16");

	// Tooltips setup
	/*CEGUI::System::getSingleton().setDefaultTooltip("TaharezLook/Tooltip");
	CEGUI::Tooltip* tooltip = CEGUI::System::getSingleton().getDefaultTooltip();
	tooltip->setHoverTime(0.1f); 
	tooltip->setDisplayTime(10.0f);
	tooltip->setFadeTime(1.0f);*/

	m_bResource = true;
}

//-------------------------------------------------------------------------------------------
//	Set up UI images. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::SetupImageset()
{
	if(m_bImageset)
		return;

	CEGUI::Imageset& loadingScreenImage = CEGUI::ImagesetManager::getSingleton().createFromImageFile("LS_Background", "Loading_Screen.jpg");
	CEGUI::Imageset& villagerBar = CEGUI::ImagesetManager::getSingleton().createFromImageFile("HUD_VillagerBar", "VillagerBar.jpg");
	
	CEGUI::Imageset& villagerOnRoadIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("VillagerOnRoad", "YellowCircle.png");
	
	CEGUI::Imageset& mainMenuImage = CEGUI::ImagesetManager::getSingleton().createFromImageFile("MM_Background", "MainMenuBack.jpg");
	
	CEGUI::Imageset& stopIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("StopIcon", "StopIcon.png");
	
	CEGUI::Imageset& explosionIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("ExplosionIcon", "Explosion.png");
	
	CEGUI::Imageset& bombIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("BombIcon", "Bomb.png");

	CEGUI::Imageset& buttonBackgroundIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("ButtonBackground", "ButtonBackground.png");
	buttonBackgroundIcon.defineImage("BlueBackground", CEGUI::Point(12.0f,10.0f), CEGUI::Size(172.0f, 171.0f), CEGUI::Point(0.0f, 0.0f));

	CEGUI::Imageset& fireIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("FireIcon", "Fire.png");
	fireIcon.defineImage("FireN", CEGUI::Point(42.0f,27.0f), CEGUI::Size(89.0f, 89.0f), CEGUI::Point(0.0f, 0.0f));
	fireIcon.defineImage("FireH", CEGUI::Point(353.0f,18.0f), CEGUI::Size(68.0f, 120.0f), CEGUI::Point(0.0f, 0.0f));

	CEGUI::Imageset& arrowNormalIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("ArrowN", "Shooter.png");
	CEGUI::Imageset& arrowHoverIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("ArrowH", "Shooter2.png");

	CEGUI::Imageset& cannonNormalIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("CannonN", "Cannon.png");
	CEGUI::Imageset& cannonHoverIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("CannonH", "Cannon2.png");

	CEGUI::Imageset& sniperNormalIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("SniperN", "Sniper.png");
	CEGUI::Imageset& sniperHoverIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("SniperH", "Sniper2.png");

	CEGUI::Imageset& slowNormalIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("SlowN", "Ice.png");
	CEGUI::Imageset& slowHoverIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("SlowH", "Ice2.png");

	CEGUI::Imageset& targetingNormalIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("TargetingN", "Targeting.png");
	CEGUI::Imageset& targetingHoverIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("TargetingH", "Targeting2.png");
	CEGUI::Imageset& targetingMouseCursor = CEGUI::ImagesetManager::getSingleton().createFromImageFile("TargetingMouseCursor", "TargetingMouseCursor.png");

	CEGUI::Imageset& upgradeNormalIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("UpgradeN", "UpArrow.png");
	CEGUI::Imageset& upgradeHoverIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("UpgradeH", "UpArrow2.png");

	CEGUI::Imageset& salvageNormalIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("SalvageN", "Salvage.png");
	CEGUI::Imageset& salvageHoverIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("SalvageH", "Salvage2.png");
	
	CEGUI::Imageset& cancelIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("Cancel", "Cancel.png");
	
	CEGUI::Imageset& costBarBackgroundIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("CostBarBackground", "CostBarBackground.jpg");
	
	CEGUI::Imageset& loadDataMenuBackIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("LoadDataMenuBack", "LoadDataMenuBack.jpg");
	CEGUI::Imageset& saveDataMenuBackIcon = CEGUI::ImagesetManager::getSingleton().createFromImageFile("SaveDataMenuBack", "SaveDataMenuBack.jpg");

	CEGUI::Imageset& essentialImages = CEGUI::ImagesetManager::getSingleton().createFromImageFile("Essentials", "EssentialImages.png");
	essentialImages.defineImage("PauseButtonNormal", CEGUI::Point(222.0f,16.0f), CEGUI::Size(173.0f, 188.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("PauseButtonHover", CEGUI::Point(27.0f,15.0f), CEGUI::Size(156.0f, 182.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("StartWaveButtonNormal", CEGUI::Point(673.0f,16.0f), CEGUI::Size(194.0f, 190.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("StartWaveButtonHover", CEGUI::Point(441.0f,16.0f), CEGUI::Size(189.0f, 188.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("SpeedUpButtonNormal", CEGUI::Point(1212.0f,16.0f), CEGUI::Size(262.0f, 200.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("SpeedUpButtonHover", CEGUI::Point(908.0f,15.0f), CEGUI::Size(260.0f, 203.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("SlowDownButtonNormal", CEGUI::Point(1784.0f,25.0f), CEGUI::Size(252.0f, 197.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("SlowDownButtonHover", CEGUI::Point(1506.0f,20.0f), CEGUI::Size(265.0f, 199.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("CoinIcon", CEGUI::Point(19.0f,228.0f), CEGUI::Size(161.0f, 174.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("WoodIcon", CEGUI::Point(229.0f,226.0f), CEGUI::Size(227.0f, 196.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("StoneIcon", CEGUI::Point(520.0f,225.0f), CEGUI::Size(226.0f, 209.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("IronIcon", CEGUI::Point(834.0f,238.0f), CEGUI::Size(241.0f, 211.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("EnemyBar", CEGUI::Point(30.0f,511.0f), CEGUI::Size(1490.0f, 180.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("Easy", CEGUI::Point(1116.0f,219.0f), CEGUI::Size(130.0f, 126.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("FastEasy", CEGUI::Point(1116.0f,219.0f), CEGUI::Size(211.0f, 127.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("MultiEasy", CEGUI::Point(1617.0f,224.0f), CEGUI::Size(122.0f, 116.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("MultiFastEasy", CEGUI::Point(1617.0f,224.0f), CEGUI::Size(203.0f, 118.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("Median", CEGUI::Point(1111.0f,365.0f), CEGUI::Size(139.0f, 131.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("FastMedian", CEGUI::Point(1111.0f,365.0f), CEGUI::Size(221.0f, 131.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("MultiMedian", CEGUI::Point(1619.0f,373.0f), CEGUI::Size(120.0f, 117.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("MultiFastMedian", CEGUI::Point(1619.0f,373.0f), CEGUI::Size(201.0f, 117.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("Hard", CEGUI::Point(1363.0f,215.0f), CEGUI::Size(141.0f, 133.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("FastHard", CEGUI::Point(1363.0f,215.0f), CEGUI::Size(222.0f, 133.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("MultiHard", CEGUI::Point(1849.0f,223.0f), CEGUI::Size(122.0f, 121.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("MultiFastHard", CEGUI::Point(1849.0f,223.0f), CEGUI::Size(189.0f, 121.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("Special", CEGUI::Point(1359.0f,363.0f), CEGUI::Size(151.0f, 137.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("FastSpecial", CEGUI::Point(1359.0f,363.0f), CEGUI::Size(220.0f, 137.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("MultiSpecial", CEGUI::Point(1851.0f,372.0f), CEGUI::Size(123.0f, 118.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("MultiFastSpecial", CEGUI::Point(1851.0f,372.0f), CEGUI::Size(191.0f, 118.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("FireTowerNormal", CEGUI::Point(55.0f,755.0f), CEGUI::Size(201.0f, 252.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("CannonTowerNormal", CEGUI::Point(329.0f,755.0f), CEGUI::Size(206.0f, 259.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("ArrowTowerNormal", CEGUI::Point(601.0f,755.0f), CEGUI::Size(203.0f, 258.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("SniperTowerNormal", CEGUI::Point(874.0f,755.0f), CEGUI::Size(205.0f, 257.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("SlowTowerNormal", CEGUI::Point(1159.0f,755.0f), CEGUI::Size(200.0f, 255.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("FireTowerHover", CEGUI::Point(50.0f,1098.0f), CEGUI::Size(205.0f, 256.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("CannonTowerHover", CEGUI::Point(328.0f,1098.0f), CEGUI::Size(205.0f, 260.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("ArrowTowerHover", CEGUI::Point(601.0f,1098.0f), CEGUI::Size(202.0f, 256.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("SniperTowerHover", CEGUI::Point(874.0f,1098.0f), CEGUI::Size(203.0f, 256.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("SlowTowerHover", CEGUI::Point(1157.0f,1098.0f), CEGUI::Size(201.0f, 253.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("SalvageNormal", CEGUI::Point(1545.0f,541.0f), CEGUI::Size(204.0f, 255.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("SalvageHover", CEGUI::Point(1791.0f,544.0f), CEGUI::Size(205.0f, 257.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("InfoNormal", CEGUI::Point(1805.0f,1489.0f), CEGUI::Size(207.0f, 257.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("InfoHover", CEGUI::Point(1808.0f,1774.0f), CEGUI::Size(204.0f, 255.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("ArrowTowerNormal2", CEGUI::Point(14.0f,1410.0f), CEGUI::Size(204.0f, 256.0f), CEGUI::Point(0.0f, 0.0f));
	essentialImages.defineImage("ArrowTowerHover2", CEGUI::Point(13.0f,1749.0f), CEGUI::Size(204.0f, 255.0f), CEGUI::Point(0.0f, 0.0f));

	m_bImageset = true;
}

//-------------------------------------------------------------------------------------------
//	Initialize UI. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::UIInit()
{
	if(m_bUIInit)
		return;

	UIManager::CreateRootWindow();

	UIManager::CreateLoadingScreen();
	UIManager::SwitchWindows("LoadingScreen");
	LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.2);

	UIManager::CreateDebugMenu("DebugMenu.txt");

	LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.1);

	UIManager::CreateInGameHUD();
	LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.1);

	UIManager::CreatePauseMenu();
	LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.1);

	UIManager::CreateMainMenu();
	LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.1);

	UIManager::CreateTileMenu();
	LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.1);
	
	UIManager::CreateTowerMenu();
	LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.1);

	UIManager::CreateOptionMenu();
	LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.1);

	UIManager::CreateLoadDataMenu();
	LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.05);

	UIManager::CreateSaveDataMenu();
	LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.05);

	m_bUIInit = true;

}

void UIManager::Update()
{
	while(VillagerManager::GetVillagerCount() != m_iVillagerCount)
	{
		if(VillagerManager::GetVillagerCount() > m_iVillagerCount)
		{
			m_iVillagerCount++;
			CEGUI::WindowManager::getSingleton().getWindow(("v"+static_cast<std::ostringstream*>( &(std::ostringstream() <<(m_iVillagerCount-1)))->str()))->setProperty("Image", "set:Essentials image:Easy ");
		}

		else if(VillagerManager::GetVillagerCount() < m_iVillagerCount)
		{
			m_iVillagerCount--;
			CEGUI::WindowManager::getSingleton().getWindow(("v"+static_cast<std::ostringstream*>( &(std::ostringstream() <<m_iVillagerCount))->str()))->setProperty("Image", "set:VillagerOnRoad image:full_image ");
		}
	}

	while(VillagerManager::GetNumVillagersStolen() != m_iVillagerStolen)
	{
		m_iVillagerStolen++;
		CEGUI::WindowManager::getSingleton().getWindow(("v"+static_cast<std::ostringstream*>( &(std::ostringstream() <<(20-m_iVillagerStolen)))->str()))->setProperty("Image", "set:Essentials image:Hard ");
	}
	
}

//-------------------------------------------------------------------------------------------
//	Switch between two windows. Switch with one window with current top window by default.
//	(By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::SwitchWindows(const std::string& p_strWindowToFront, const std::string& p_strWindowToBack)
{
	std::map<std::string, BaseWindow*>::iterator windowToFront = m_mapWindows.find(p_strWindowToFront);
	std::map<std::string, BaseWindow*>::iterator windowToBack = m_mapWindows.find(p_strWindowToBack);

	if(windowToBack != m_mapWindows.end())
		windowToBack->second->PushToBack();

	if(windowToFront != m_mapWindows.end())
		windowToFront->second->MoveToTop();
		
	m_strCurrentTopWindow = p_strWindowToFront;
}

//-------------------------------------------------------------------------------------------
//	Create default window. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::CreateRootWindow()
{
	CEGUI::Window *rootWindow = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "RootWindow");
	CEGUI::System::getSingleton().setGUISheet(rootWindow);

	m_strCurrentTopWindow = "RootWindow";
}

//-------------------------------------------------------------------------------------------
//	Create game logo which shows up when the game initializing. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::CreateGameLogo()
{

}

//-------------------------------------------------------------------------------------------
//	Create loading screen. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::CreateLoadingScreen()
{
	LoadingScreen::GetSingletonPtr()->Init();

	m_mapWindows["LoadingScreen"] = LoadingScreen::GetSingletonPtr();
}

//-------------------------------------------------------------------------------------------
//	Create main menu. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::CreateMainMenu()
{
	MainMenu::GetSingletonPtr()->Init();

	m_mapWindows["MainMenu"] = MainMenu::GetSingletonPtr();
}

//-------------------------------------------------------------------------------------------
//	Create debug menu. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::CreateDebugMenu(const std::string& p_strFileName)
{
	CEGUI::Window *debugMenu = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText", "DebugMenu");
	debugMenu->setAlpha(0.5);
	debugMenu->setText("");
	ReadDebugMenu(p_strFileName, debugMenu);
	debugMenu->setVisible(true);
	debugMenu->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	debugMenu->setSize(CEGUI::UVector2(CEGUI::UDim(0.21, 0), CEGUI::UDim(0.75, 0)));
	debugMenu->moveInFront(CEGUI::WindowManager::getSingleton().getWindow("RootWindow"));

	CEGUI::System::getSingleton().getGUISheet()->addChildWindow(debugMenu);
}

//---------------------------------------------------------------------------------------------
//	Read a file in by given file name and append them to a debug menu. (By Pierre)
//---------------------------------------------------------------------------------------------
bool UIManager::ReadDebugMenu(const std::string& p_strFileName, CEGUI::Window* p_pWindow)
{
	std::ifstream file;
	std::string line;

	file.open(p_strFileName);
	if(file.is_open())
	{
		while(!file.eof())
		{
			getline(file, line);
			p_pWindow->appendText(line);
			p_pWindow->appendText("\n");
		}
	}
	else
		return false;

	file.close();

	return true;
}

//-------------------------------------------------------------------------------------------
//	Create in game HUD. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::CreateInGameHUD()
{
	InGameHUD::GetSingletonPtr()->Init();

	m_mapWindows["InGameHUD"] = InGameHUD::GetSingletonPtr();
}

//-------------------------------------------------------------------------------------------
//	Create pause menu. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::CreatePauseMenu()
{
	PauseMenu::GetSingletonPtr()->Init();

	m_mapWindows["PauseMenu"] = PauseMenu::GetSingletonPtr();
}

//-------------------------------------------------------------------------------------------
//	Create tile radial menu. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::CreateTileMenu()
{
	TileMenu::GetSingletonPtr()->Init();

	m_mapWindows["TileMenu"] = TileMenu::GetSingletonPtr();
}

//-------------------------------------------------------------------------------------------
//	Create tower radial menu. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::CreateTowerMenu()
{
	TowerMenu::GetSingletonPtr()->Init();

	m_mapWindows["TowerMenu"] = TowerMenu::GetSingletonPtr();
}

//-------------------------------------------------------------------------------------------
//	Create option menu. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::CreateOptionMenu()
{
	OptionMenu::GetSingletonPtr()->Init();

	m_mapWindows["OptionMenu"] = OptionMenu::GetSingletonPtr();
}

//-------------------------------------------------------------------------------------------
//	Create load data menu. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::CreateLoadDataMenu()
{
	LoadDataMenu::GetSingletonPtr()->Init();

	m_mapWindows["LoadDataMenu"] = LoadDataMenu::GetSingletonPtr();
}

//-------------------------------------------------------------------------------------------
//	Create save data menu. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::CreateSaveDataMenu()
{
	SaveDataMenu::GetSingletonPtr()->Init();

	m_mapWindows["SaveDataMenu"] = SaveDataMenu::GetSingletonPtr();
}

//-------------------------------------------------------------------------------------------
//	Get specific window. (By Pierre)
//-------------------------------------------------------------------------------------------
CEGUI::Window* UIManager::GetUIWindow(const std::string& p_strWindowName)
{
	if(m_mapWindows.find(p_strWindowName) != m_mapWindows.end())
		return m_mapWindows[p_strWindowName]->GetMainWindowPtr();

	return 0;
}

//-------------------------------------------------------------------------------------------
//	Destroy all UI. (By Pierre)
//-------------------------------------------------------------------------------------------
void UIManager::DestroyAllUI()
{
	if(!m_bUIInit)
		return;

	EnemyBar::Destroy();
	InGameHUD::Destroy();
	LoadingScreen::Destroy();
	MainMenu::Destroy();
	PauseMenu::Destroy();
	TileMenu::Destroy();
	TowerMenu::Destroy();
	OptionMenu::Destroy();
	LoadDataMenu::Destroy();

	m_mapWindows.clear();

	// Double checking memory leak for CEGUI stuff
	CEGUI::WindowManager::getSingleton().destroyAllWindows();
	delete CEGUI::AnimationManager::getSingletonPtr();
}