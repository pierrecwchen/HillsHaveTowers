//-------------------------------------------------------------------------------------------
//	MainMenu.cpp											Original Author: Pierre Chen
//
//	Please see notes from MainMenu.h
//-------------------------------------------------------------------------------------------

#include "MainMenu.h"
#include "states\StateManager.h"
#include "states\PrototypeState.h"
#include "states\TerrainTestState.h"
#include "states\GameState.h"
#include "SoundManager.h"
#include "UIManager.h"

MainMenu* MainMenu::m_mmMainMenu = 0;

//-------------------------------------------------------------------------------------------
//	Private defult constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
MainMenu::MainMenu(): m_bQuit(false)
{
}

//-------------------------------------------------------------------------------------------
//	Private copy constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
MainMenu::MainMenu(const MainMenu& p_wMainMenu)
{
}

//-------------------------------------------------------------------------------------------
//	Private destructor. (By Pierre)
//-------------------------------------------------------------------------------------------
MainMenu::~MainMenu()
{
	CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(m_pButtonHoverAnim);
	CEGUI::AnimationManager::getSingleton().destroyAnimation(m_pButtonHoverAnim);
	CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(m_pButtonFlyInAnimL);
	CEGUI::AnimationManager::getSingleton().destroyAnimation(m_pButtonFlyInAnimL);
	CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(m_pButtonFlyInAnimR);
	CEGUI::AnimationManager::getSingleton().destroyAnimation(m_pButtonFlyInAnimR);

	m_pMainWindow = 0;
	m_pStartButton = 0;
	m_pLoadButton = 0;
	m_pOptionButton = 0;
	m_pQuitButton = 0;
	m_pPrototypeButton = 0;
	m_pTerrainButton = 0;

	m_pStartButtonH = 0;
	m_pStartButtonFIn = 0;
	m_pLoadButtonH = 0;
	m_pLoadButtonFIn = 0;
	m_pOptionButtonH = 0;
	m_pOptionButtonFIn = 0;
	m_pQuitButtonH = 0;
	m_pQuitButtonFIn = 0;

	m_pButtonHoverAnim = 0;
	m_pButtonFlyInAnimL = 0;
}

//-------------------------------------------------------------------------------------------
//	Private assignment operator. (By Pierre)
//-------------------------------------------------------------------------------------------
void MainMenu::operator=(const MainMenu& p_wMainMenu)
{
}

//-------------------------------------------------------------------------------------------
//	Return main menu singleton. (By Pierre)
//-------------------------------------------------------------------------------------------
MainMenu* MainMenu::GetSingletonPtr()
{
	if(!m_mmMainMenu)
	{
		m_mmMainMenu = new MainMenu();
	}

	return m_mmMainMenu;
}

//-------------------------------------------------------------------------------------------
//	Public destructor. (By Pierre)
//-------------------------------------------------------------------------------------------
void MainMenu::Destroy()
{
	delete m_mmMainMenu;
}

//-------------------------------------------------------------------------------------------
//	Initialize the main menu. (By Pierre)
//-------------------------------------------------------------------------------------------
void MainMenu::Init()
{
	if(m_bInitialized)
		return;

	// Create background image
	m_pMainWindow = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage", "MainMenu");
	m_pMainWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	m_pMainWindow->setSize(CEGUI::UVector2(CEGUI::UDim(1,0), CEGUI::UDim(1,0)));
	m_pMainWindow->setProperty("Image", "set:MM_Background image:full_image");
	m_pMainWindow->setProperty("FrameEnabled", "false");

	// Create start button
	m_pStartButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "MM_Start");
	m_pStartButton->setText("Start");
	m_pStartButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35,0), CEGUI::UDim(0.4,0)));
	m_pStartButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.07,0)));
	
	m_pMainWindow->addChildWindow(m_pStartButton);
	m_pStartButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenu::StartGame, MainMenu::GetSingletonPtr()));
	m_pStartButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&MainMenu::StartGameHover, MainMenu::GetSingletonPtr()));

	// Create load button
	m_pLoadButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "MM_Load");
	m_pLoadButton->setText("Load");
	m_pLoadButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35,0), CEGUI::UDim(0.5,0)));
	m_pLoadButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.07,0)));
	
	m_pMainWindow->addChildWindow(m_pLoadButton);
	m_pLoadButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenu::LoadGame, MainMenu::GetSingletonPtr()));
	m_pLoadButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&MainMenu::LoadGameHover, MainMenu::GetSingletonPtr()));

	// Create option button
	m_pOptionButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "MM_Option");
	m_pOptionButton->setText("Option");
	m_pOptionButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35,0), CEGUI::UDim(0.6,0)));
	m_pOptionButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.07,0)));
	
	m_pMainWindow->addChildWindow(m_pOptionButton);
	m_pOptionButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenu::Option, MainMenu::GetSingletonPtr()));
	m_pOptionButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&MainMenu::OptionHover, MainMenu::GetSingletonPtr()));

	// Create quit button
	m_pQuitButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "MM_Quit");
	m_pQuitButton->setText("Quit");
	m_pQuitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35,0), CEGUI::UDim(0.7,0)));
	m_pQuitButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.07,0)));
	
	m_pMainWindow->addChildWindow(m_pQuitButton);
	m_pQuitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenu::QuitGame, MainMenu::GetSingletonPtr()));
	m_pQuitButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&MainMenu::QuitGameHover, MainMenu::GetSingletonPtr()));

	// Create prototype button
	m_pPrototypeButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "MM_Prototype");
	m_pPrototypeButton->setText("Prototype");
	m_pPrototypeButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35,0), CEGUI::UDim(0.8,0)));
	m_pPrototypeButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.07,0)));
	
	m_pMainWindow->addChildWindow(m_pPrototypeButton);
	m_pPrototypeButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenu::PrototypeState, MainMenu::GetSingletonPtr()));

	// Create terrain test button
	m_pTerrainButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "MM_TerrainTest");
	m_pTerrainButton->setText("Terrain Test");
	m_pTerrainButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35,0), CEGUI::UDim(0.9,0)));
	m_pTerrainButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.07,0)));
	
	m_pMainWindow->addChildWindow(m_pTerrainButton);
	m_pTerrainButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenu::TerrainTestState, MainMenu::GetSingletonPtr()));

	CEGUI::System::getSingleton().getGUISheet()->addChildWindow(m_pMainWindow);

	// Create animation definition for hover a button
	m_pButtonHoverAnim = CEGUI::AnimationManager::getSingleton().createAnimation("MM_ButtonHover");
	m_pButtonHoverAnim->setDuration(1.5f);
	m_pButtonHoverAnim->setReplayMode(CEGUI::Animation::RM_Once);

	CEGUI::Affector* affector = m_pButtonHoverAnim->createAffector("Alpha", "float");
	affector->createKeyFrame(0.0f, "1.0");
	affector->createKeyFrame(0.75f, "0.5");
	affector->createKeyFrame(1.5f, "1.0");

	m_pStartButtonH = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pButtonHoverAnim);
	m_pStartButtonH->setTargetWindow(m_pStartButton);
	m_pLoadButtonH = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pButtonHoverAnim);
	m_pLoadButtonH->setTargetWindow(m_pLoadButton);
	m_pOptionButtonH = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pButtonHoverAnim);
	m_pOptionButtonH->setTargetWindow(m_pOptionButton);
	m_pQuitButtonH = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pButtonHoverAnim);
	m_pQuitButtonH->setTargetWindow(m_pQuitButton);

	// Create fly in from left animation
	m_pButtonFlyInAnimL = CEGUI::AnimationManager::getSingleton().createAnimation("MM_ButtonFlyInL");
	m_pButtonFlyInAnimL->setDuration(1.0f);
	m_pButtonFlyInAnimL->setReplayMode(CEGUI::Animation::RM_Once);
	
	affector = m_pButtonFlyInAnimL->createAffector("UnifiedXPosition", "UDim");
	affector->createKeyFrame(0.0f, "{-0.3,0.0}");
	affector->createKeyFrame(1.0f, "{0.35,0.0}", CEGUI::KeyFrame::Progression::P_QuadraticDecelerating);
	affector = m_pButtonFlyInAnimL->createAffector("XRotation", "float");
	affector->createKeyFrame(0.0f, "0.0");
	affector->createKeyFrame(1.0f, "360.0", CEGUI::KeyFrame::Progression::P_QuadraticDecelerating);

	// Create fly in from right animation
	m_pButtonFlyInAnimR = CEGUI::AnimationManager::getSingleton().createAnimation("MM_ButtonFlyInR");
	m_pButtonFlyInAnimR->setDuration(1.0f);
	m_pButtonFlyInAnimR->setReplayMode(CEGUI::Animation::RM_Once);
	
	affector = m_pButtonFlyInAnimR->createAffector("UnifiedXPosition", "UDim");
	affector->createKeyFrame(0.0f, "{1.0,0.0}");
	affector->createKeyFrame(1.0f, "{0.35,0.0}", CEGUI::KeyFrame::Progression::P_QuadraticDecelerating);
	affector = m_pButtonFlyInAnimR->createAffector("XRotation", "float");
	affector->createKeyFrame(0.0f, "0.0");
	affector->createKeyFrame(1.0f, "360.0", CEGUI::KeyFrame::Progression::P_QuadraticDecelerating);

	// Bind animation to buttons
	m_pStartButtonFIn = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pButtonFlyInAnimL);
	m_pStartButtonFIn->setTargetWindow(m_pStartButton);
	m_pLoadButtonFIn = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pButtonFlyInAnimR);
	m_pLoadButtonFIn->setTargetWindow(m_pLoadButton);
	m_pOptionButtonFIn = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pButtonFlyInAnimL);
	m_pOptionButtonFIn->setTargetWindow(m_pOptionButton);
	m_pQuitButtonFIn = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pButtonFlyInAnimR);
	m_pQuitButtonFIn->setTargetWindow(m_pQuitButton);

	m_bInitialized = true;

	PushToBack();
}

void MainMenu::MoveToTop()
{
	BaseWindow::MoveToTop();
    SoundManager::PlayMusic("concertVariations.wav");

	m_pStartButtonFIn->start();
	m_pLoadButtonFIn->start();
	m_pOptionButtonFIn->start();
	m_pQuitButtonFIn->start();
}

void MainMenu::PushToBack()
{
	BaseWindow::PushToBack();
}

//-------------------------------------------------------------------------------------------
//	Function used when start button clicked. (By Pierre)
//-------------------------------------------------------------------------------------------
bool MainMenu::StartGame(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonPress.wav");
	Hills::StateManager::SwitchState("Game");

	return true;
}

//-------------------------------------------------------------------------------------------
//	Function used when load  button clicked. (By Pierre)
//-------------------------------------------------------------------------------------------
bool MainMenu::LoadGame(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonPress.wav");
	LoadDataMenu::GetSingletonPtr()->MoveToTop();
	m_pMainWindow->disable();
	return true;
}

//-------------------------------------------------------------------------------------------
//	Function used when option  button clicked. (By Pierre)
//-------------------------------------------------------------------------------------------
bool MainMenu::Option(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonPress.wav");
	return true;
}

//-------------------------------------------------------------------------------------------
//	Function used when quit button clicked. (By Pierre)
//-------------------------------------------------------------------------------------------
bool MainMenu::QuitGame(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonPress.wav");
	m_bQuit = true;

	return true;
}

bool MainMenu::StartGameHover(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonHover.wav");

	if(m_pStartButtonH->isRunning())
		return true;

    
	m_pStartButtonH->start();
	
	return true;
}

bool MainMenu::LoadGameHover(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonHover.wav");

	if(m_pLoadButtonH->isRunning())
		return true;
	
   
	m_pLoadButtonH->start();
	
	return true;
}

bool MainMenu::OptionHover(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonHover.wav");

	if(m_pOptionButtonH->isRunning())
		return true;

	m_pOptionButtonH->start();
	
	return true;
}

bool MainMenu::QuitGameHover(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonHover.wav");

	if(m_pQuitButtonH->isRunning())
		return true;
	
	m_pQuitButtonH->start();
	
	return true;
}

//-------------------------------------------------------------------------------------------
//	Function used when prototype button clicked. (By Pierre)
//-------------------------------------------------------------------------------------------
bool MainMenu::PrototypeState(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonPress.wav");
	Hills::StateManager::SwitchState("Prototype");

	return true;
}

//-------------------------------------------------------------------------------------------
//	Function used when terrain test button clicked. (By Pierre)
//-------------------------------------------------------------------------------------------
bool MainMenu::TerrainTestState(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonPress.wav");
	Hills::StateManager::SwitchState("TerrainTest");

	return true;
}