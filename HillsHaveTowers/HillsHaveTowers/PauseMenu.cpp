//-------------------------------------------------------------------------------------------
//	PauseMenu.cpp											Original Author: Pierre Chen
//
//	Please see note from PauseMenu.h
//-------------------------------------------------------------------------------------------

#include "PauseMenu.h"
#include "states\StateManager.h"
#include "states\PrototypeState.h"
#include "states\GameState.h"
#include "states\TerrainTestState.h"
#include "UIManager.h"
#include "SoundManager.h"

PauseMenu* PauseMenu::m_pmPauseMenu = 0;

//-------------------------------------------------------------------------------------------
//	Private default constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
PauseMenu::PauseMenu(): m_bQuit(false)
{
}

//-------------------------------------------------------------------------------------------
//	Private copy constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
PauseMenu::PauseMenu(const PauseMenu& p_pmPauseMenu)
{
}

//-------------------------------------------------------------------------------------------
//	Private destructor. (By Pierre)
//-------------------------------------------------------------------------------------------
PauseMenu::~PauseMenu()
{
	CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(m_pPauseMenuAnim);
	CEGUI::AnimationManager::getSingleton().destroyAnimation(m_pPauseMenuAnim);
	CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(m_pBombAnim);
	CEGUI::AnimationManager::getSingleton().destroyAnimation(m_pBombAnim);
	CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(m_pPauseMenuFadeOutAnim);
	CEGUI::AnimationManager::getSingleton().destroyAnimation(m_pPauseMenuFadeOutAnim);

	m_pMainWindow = 0;
}

//-------------------------------------------------------------------------------------------
//	Private assignment operator. (By Pierre)
//-------------------------------------------------------------------------------------------
void PauseMenu::operator=(const PauseMenu& p_pmPauseMenu)
{
}

//-------------------------------------------------------------------------------------------
//	Return singleton pointer. (By Pierre)
//-------------------------------------------------------------------------------------------
PauseMenu* PauseMenu::GetSingletonPtr()
{
	if(m_pmPauseMenu == 0)
	{
		m_pmPauseMenu = new PauseMenu();
	}

	return m_pmPauseMenu;
}

//-------------------------------------------------------------------------------------------
//	Public destructor. (By Pierre)
//-------------------------------------------------------------------------------------------
void PauseMenu::Destroy()
{
	delete m_pmPauseMenu;
}

void PauseMenu::Init()
{
	if(m_bInitialized)
		return;

	// Create base pause menu
	m_pMainWindow = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "PauseMenu");
	m_pMainWindow->setSize(CEGUI::UVector2(CEGUI::UDim(1.0,0), CEGUI::UDim(1.0,0)));
	m_pMainWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));

	// Create pause menu
	m_pPauseMenu = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "PM_BackGround");
	m_pPauseMenu->setSize(CEGUI::UVector2(CEGUI::UDim(1.0,0), CEGUI::UDim(1.0,0)));
	m_pPauseMenu->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	m_pPauseMenu->setProperty("Image", "set:ExplosionIcon image:full_image");

	m_pMainWindow->addChildWindow(m_pPauseMenu);

	// Create resume game button
	CEGUI::Window* resumeGameButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "PM_Resume");
	resumeGameButton->setText("Resume Game");
	resumeGameButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.04,0)));
	resumeGameButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.32,0)));

	m_pPauseMenu->addChildWindow(resumeGameButton);
	resumeGameButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PauseMenu::ResumeGame, PauseMenu::GetSingletonPtr()));

	// Create load last check point button
	CEGUI::Window* loadLastCheckPointButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "PM_LoadLast");
	loadLastCheckPointButton->setText("Load Last Check Point");
	loadLastCheckPointButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.04,0)));
	loadLastCheckPointButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.37,0)));

	m_pPauseMenu->addChildWindow(loadLastCheckPointButton);
	loadLastCheckPointButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PauseMenu::LoadLastCheckPoint, PauseMenu::GetSingletonPtr()));

	// Create restart level button
	CEGUI::Window* RestartLevelButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "PM_Restart");
	RestartLevelButton->setText("Restart Level");
	RestartLevelButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.04,0)));
	RestartLevelButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.42,0)));

	m_pPauseMenu->addChildWindow(RestartLevelButton);
	RestartLevelButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PauseMenu::RestartLevel, PauseMenu::GetSingletonPtr()));

	// Create save game button
	CEGUI::Window* saveGameButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "PM_Save");
	saveGameButton->setText("Save Game");
	saveGameButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.04,0)));
	saveGameButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.47,0)));

	m_pPauseMenu->addChildWindow(saveGameButton);
	saveGameButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PauseMenu::SaveGame, PauseMenu::GetSingletonPtr()));

	// Create load game button
	CEGUI::Window* loadGameButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "PM_Load");
	loadGameButton->setText("Load Game");
	loadGameButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.04,0)));
	loadGameButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.52,0)));

	m_pPauseMenu->addChildWindow(loadGameButton);
	loadGameButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PauseMenu::LoadGame, PauseMenu::GetSingletonPtr()));

	// Create setting button
	CEGUI::Window* settingButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "PM_Setting");
	settingButton->setText("Settings");
	settingButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.04,0)));
	settingButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.57,0)));

	m_pPauseMenu->addChildWindow(settingButton);
	settingButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PauseMenu::Settings, PauseMenu::GetSingletonPtr()));

	// Create exit to main menu button
	CEGUI::Window* mainMenuButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "PM_MainMenu");
	mainMenuButton->setText("Exit To Main Menu");
	mainMenuButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.04,0)));
	mainMenuButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.62,0)));

	m_pPauseMenu->addChildWindow(mainMenuButton);
	mainMenuButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PauseMenu::ExitToMainMenu, PauseMenu::GetSingletonPtr()));

	// Create exit game button
	CEGUI::Window* exitGameButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "PM_Exit");
	exitGameButton->setText("Exit Game");
	exitGameButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.04,0)));
	exitGameButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.67,0)));

	m_pPauseMenu->addChildWindow(exitGameButton);
	exitGameButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PauseMenu::ExitGame, PauseMenu::GetSingletonPtr()));


	// Add the pause menu to the root
	CEGUI::System::getSingleton().getGUISheet()->addChildWindow(m_pMainWindow);


	// Create pause menu animation
	// Show up animation
	m_pPauseMenuAnim = CEGUI::AnimationManager::getSingleton().createAnimation("PM_Show");
	m_pPauseMenuAnim->setDuration(0.5f);
	m_pPauseMenuAnim->setReplayMode(CEGUI::Animation::RM_Once);

	CEGUI::Affector* affector = m_pPauseMenuAnim->createAffector("UnifiedAreaRect", "URect");
	affector->createKeyFrame(0.0f, "{{0.5,0.0}, {0.5,0.0}, {0.5,0.0}, {0.5,0.0}}");
	affector->createKeyFrame(0.25f, "{{0.5,0.0}, {0.5,0.0}, {0.5,0.0}, {0.5,0.0}}");
	affector->createKeyFrame(0.5f, "{{0.0,0.0}, {0.0,0.0}, {1.0,0.0}, {1.0,0.0}}", CEGUI::KeyFrame::Progression::P_QuadraticAccelerating);

	m_pPauseMenuResize = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pPauseMenuAnim);
	m_pPauseMenuResize->setTargetWindow(m_pPauseMenu);

	// Fade out animation
	m_pPauseMenuFadeOutAnim = CEGUI::AnimationManager::getSingleton().createAnimation("PM_FadeOut");
	m_pPauseMenuFadeOutAnim->setDuration(0.5f);
	m_pPauseMenuFadeOutAnim->setReplayMode(CEGUI::Animation::RM_Once);

	affector = m_pPauseMenuFadeOutAnim->createAffector("Alpha", "float");
	affector->createKeyFrame(0.0f, "1.0");
	affector->createKeyFrame(0.25f, "0.0");

	affector = m_pPauseMenuFadeOutAnim->createAffector("AlwaysOnTop", "bool");
	affector->createKeyFrame(0.0f, "true");
	affector->createKeyFrame(0.5f, "false");

	affector = m_pPauseMenuFadeOutAnim->createAffector("Visible", "bool");
	affector->createKeyFrame(0.0f, "true");
	affector->createKeyFrame(0.5f, "false");

	affector = m_pPauseMenuFadeOutAnim->createAffector("Disabled", "bool");
	affector->createKeyFrame(0.0f, "false");
	affector->createKeyFrame(0.5f, "true");

	m_pPauseMenuFadeOut = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pPauseMenuFadeOutAnim);
	m_pPauseMenuFadeOut->setTargetWindow(m_pMainWindow);


	// Create bomb icon and its animation
	CEGUI::Window* bomb = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "PM_Bomb");
	bomb->setSize(CEGUI::UVector2(CEGUI::UDim(0.1,0), CEGUI::UDim(0.1,0)));
	bomb->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	bomb->setProperty("Image", "set:BombIcon image:full_image");

	m_pMainWindow->addChildWindow(bomb);

	m_pBombAnim = CEGUI::AnimationManager::getSingleton().createAnimation("PM_Bomb");
	m_pBombAnim->setDuration(0.5f);
	m_pBombAnim->setReplayMode(CEGUI::Animation::RM_Once);

	affector = m_pBombAnim->createAffector("UnifiedXPosition", "UDim");
	affector->createKeyFrame(0.0f, "{-0.1,0.0}");
	affector->createKeyFrame(0.25f, "{0.4,0.0}", CEGUI::KeyFrame::Progression::P_QuadraticAccelerating);

	affector = m_pBombAnim->createAffector("UnifiedYPosition", "UDim");
	affector->createKeyFrame(0.0f, "{0.3,0.0}");
	affector->createKeyFrame(0.1f, "{0.0,0.0}", CEGUI::KeyFrame::Progression::P_QuadraticDecelerating);
	affector->createKeyFrame(0.25f, "{0.4,0.0}", CEGUI::KeyFrame::Progression::P_QuadraticAccelerating);

	affector = m_pBombAnim->createAffector("Visible", "bool");
	affector->createKeyFrame(0.0f, "true");
	affector->createKeyFrame(0.5f, "false");

	m_pBombFlyIn = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pBombAnim);
	m_pBombFlyIn->setTargetWindow(bomb);

	m_bInitialized = true;

	BaseWindow::PushToBack();
}

void PauseMenu::MoveToTop()
{
	BaseWindow::MoveToTop();

	if(m_pPauseMenuFadeOut->isRunning())
		m_pPauseMenuFadeOut->stop();

	if(m_pMainWindow->getAlpha() != 1.0)
		m_pMainWindow->setAlpha(1.0);

	m_pBombFlyIn->start();
	m_pPauseMenuResize->start();
}

void PauseMenu::PushToBack()
{
	if(!m_bInitialized || !m_bIsShown)
		return;

	m_pPauseMenuFadeOut->start();

	m_bIsShown = false;
}

void PauseMenu::ResumeGame()
{
	UIManager::SwitchWindows("InGameHUD");

    SoundManager::ResumeAllSounds();
    SoundManager::RaiseMusicVolume();

	if(dynamic_cast<Hills::TerrainTestState*>(Hills::StateManager::GetCurrentState()) != 0)
		return;

	EnemyBar::GetSingletonPtr()->Restart();
	(dynamic_cast<Hills::PrototypeState*>(Hills::StateManager::GetCurrentState()) != 0)?
		dynamic_cast<Hills::PrototypeState*>(Hills::StateManager::GetCurrentState())->SetPause(false) :
		dynamic_cast<Hills::GameState*>(Hills::StateManager::GetCurrentState())->SetPause(false);
}

bool PauseMenu::ResumeGame(const CEGUI::EventArgs &p_eEventArgs)
{
	ResumeGame();

	return true;
}

bool PauseMenu::LoadLastCheckPoint(const CEGUI::EventArgs &p_eEventArgs)
{
	return true;
}

bool PauseMenu::RestartLevel(const CEGUI::EventArgs &p_eEventArgs)
{
	Hills::StateManager::GetCurrentState()->Destroy();
	Hills::StateManager::GetCurrentState()->Init();

	return true;
}

bool PauseMenu::SaveGame(const CEGUI::EventArgs &p_eEventArgs)
{
	UIManager::SwitchWindows("SaveDataMenu");
	return true;
}

bool PauseMenu::LoadGame(const CEGUI::EventArgs &p_eEventArgs)
{
	UIManager::SwitchWindows("LoadDataMenu");
	return true;
}

bool PauseMenu::Settings(const CEGUI::EventArgs &p_eEventArgs)
{
	return true;
}

bool PauseMenu::ExitToMainMenu(const CEGUI::EventArgs &p_eEventArgs)
{
	Hills::StateManager::SwitchState("FrontEnd");
    SoundManager::RaiseMusicVolume();

	return true;
}

bool PauseMenu::ExitGame(const CEGUI::EventArgs &p_eEventArgs)
{
	m_bQuit = true;
    SoundManager::StopAllSounds();

	return true;
}