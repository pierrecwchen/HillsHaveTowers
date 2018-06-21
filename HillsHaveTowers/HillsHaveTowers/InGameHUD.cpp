//-------------------------------------------------------------------------------------------
//	InGameHUD.cpp											Original Author: Pierre Chen
//
//	Please see notes from InGameHUD.h
//-------------------------------------------------------------------------------------------

#include "InGameHUD.h"
#include "states\StateManager.h"
#include "states\PrototypeState.h"
#include "states\GameState.h"
#include "states\TerrainTestState.h"
#include <iostream>
#include "UIManager.h"
#include "SelectableManager.h"
#include "SoundManager.h"

InGameHUD* InGameHUD::m_uiInGameHUD = 0;

//-------------------------------------------------------------------------------------------
//	Private default constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
InGameHUD::InGameHUD(): m_iCoinStats(0),
						m_iWoodStats(0),
						m_iStoneStats(0),
						m_iIronStats(0)
{
}

//-------------------------------------------------------------------------------------------
//	Private copy constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
InGameHUD::InGameHUD(const InGameHUD& p_uiInGameHUD)
{
}

//-------------------------------------------------------------------------------------------
//	Private destructor. (By Pierre)
//-------------------------------------------------------------------------------------------
InGameHUD::~InGameHUD()
{
	CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(m_pButtonHoverAnim);
	CEGUI::AnimationManager::getSingleton().destroyAnimation(m_pButtonHoverAnim);

	m_pMainWindow = 0;
	m_pVillagerCounter = 0;
	m_pPauseButton = 0;
	m_pSlowDownButton = 0;
	m_pSpeedUpButton = 0;
	m_pStartWaveButton = 0;

	m_pPauseButtonAnim = 0;
	m_pSlowDownButtonAnim = 0;
	m_pSpeedUpButtonAnim = 0;
	m_pStartWaveButtonAnim = 0;

	m_pButtonHoverAnim = 0;
}

//-------------------------------------------------------------------------------------------
//	Private assignment operator. (By Pierre)
//-------------------------------------------------------------------------------------------
void InGameHUD::operator=(const InGameHUD& p_uiInGameHUD)
{
}

//-------------------------------------------------------------------------------------------
//	Return singleton pointer. (By Pierre)
//-------------------------------------------------------------------------------------------
InGameHUD* InGameHUD::GetSingletonPtr()
{
	if(!m_uiInGameHUD)
		m_uiInGameHUD = new InGameHUD();

	return m_uiInGameHUD;
}

//-------------------------------------------------------------------------------------------
//	Public destructor. (By Pierre)
//-------------------------------------------------------------------------------------------
void InGameHUD::Destroy()
{
	delete m_uiInGameHUD;
}

//-------------------------------------------------------------------------------------------
//	Initialize the in-game HUD. (By Pierre)
//-------------------------------------------------------------------------------------------
void InGameHUD::Init()
{
	if(m_bInitialized)
		return;

	// Create villager counter
	m_pVillagerCounter = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "Villager");
	m_pVillagerCounter->setSize(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.02,0)));
	m_pVillagerCounter->setPosition(CEGUI::UVector2(CEGUI::UDim(0.25,0), CEGUI::UDim(0.0,0)));
	m_pVillagerCounter->setProperty("Image", "set:HUD_VillagerBar image: full_image");

	CEGUI::System::getSingleton().getGUISheet()->addChildWindow(m_pVillagerCounter);

	CEGUI::Window* villager;

	for(int i = 0; i < 20; ++i)
	{
		villager = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage",("v"+static_cast<std::ostringstream*>( &(std::ostringstream() <<i))->str()));
		villager->setSize(CEGUI::UVector2(CEGUI::UDim(0.03,0), CEGUI::UDim(0.7,0)));
		villager->setPosition(CEGUI::UVector2(CEGUI::UDim(0.03*static_cast<float>(i)+0.02*static_cast<float>(i)+0.01,0), CEGUI::UDim(0.15,0)));
		villager->setProperty("Image", "set:Essentials image:Easy ");

		m_pVillagerCounter->addChildWindow(villager);
	}

	// Create in-game bar
	m_pMainWindow = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "InGameHUD");
	m_pMainWindow->setSize(CEGUI::UVector2(CEGUI::UDim(1,0), CEGUI::UDim(0.04,0)));
	m_pMainWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.96,0)));
	m_pMainWindow->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&InGameHUD::InGameHUDHover, InGameHUD::GetSingletonPtr()));
	m_pMainWindow->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&InGameHUD::InGameHUDLeaves, InGameHUD::GetSingletonPtr()));

	// Create pause button
	m_pPauseButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "HUD_PauseButton");
	m_pPauseButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.04,0), CEGUI::UDim(1,0)));
	m_pPauseButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	m_pPauseButton->setProperty( "NormalImage", "set:Essentials image:PauseButtonNormal" );
	m_pPauseButton->setProperty( "HoverImage", "set:Essentials image:PauseButtonHover" );
	m_pPauseButton->setProperty( "PushedImage", "set:Essentials image:PauseButtonHover" );

	m_pMainWindow->addChildWindow(m_pPauseButton);
	m_pPauseButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&InGameHUD::Pause, InGameHUD::GetSingletonPtr()));
	m_pPauseButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&InGameHUD::PauseHover, InGameHUD::GetSingletonPtr()));

	// Create slow down button
	m_pSlowDownButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "HUD_SlowDownButton");
	m_pSlowDownButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.04,0), CEGUI::UDim(1,0)));
	m_pSlowDownButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.05,0), CEGUI::UDim(0,0)));
	m_pSlowDownButton->setProperty( "NormalImage", "set:Essentials image:SlowDownButtonNormal" );
	m_pSlowDownButton->setProperty( "HoverImage", "set:Essentials image:SlowDownButtonHover" );
	m_pSlowDownButton->setProperty( "PushedImage", "set:Essentials image:SlowDownButtonHover" );

	m_pMainWindow->addChildWindow(m_pSlowDownButton);
	m_pSlowDownButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&InGameHUD::SlowDown, InGameHUD::GetSingletonPtr()));
	m_pSlowDownButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&InGameHUD::SlowDownHover, InGameHUD::GetSingletonPtr()));

	// Create speed up button
	m_pSpeedUpButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "HUD_SpeedUpButton");
	m_pSpeedUpButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.04,0), CEGUI::UDim(1,0)));
	m_pSpeedUpButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1,0), CEGUI::UDim(0,0)));
	m_pSpeedUpButton->setProperty( "NormalImage", "set:Essentials image:SpeedUpButtonNormal" );
	m_pSpeedUpButton->setProperty( "HoverImage", "set:Essentials image:SpeedUpButtonHover" );
	m_pSpeedUpButton->setProperty( "PushedImage", "set:Essentials image:SpeedUpButtonHover" );

	m_pMainWindow->addChildWindow(m_pSpeedUpButton);
	m_pSpeedUpButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&InGameHUD::SpeedUp, InGameHUD::GetSingletonPtr()));
	m_pSpeedUpButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&InGameHUD::SpeedUpHover, InGameHUD::GetSingletonPtr()));

	// Create start wave button
	m_pStartWaveButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "HUD_StartWaveButton");
	m_pStartWaveButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.04,0), CEGUI::UDim(1,0)));
	m_pStartWaveButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15,0), CEGUI::UDim(0,0)));
	m_pStartWaveButton->setProperty( "NormalImage", "set:Essentials image:StartWaveButtonNormal" );
	m_pStartWaveButton->setProperty( "HoverImage", "set:Essentials image:StartWaveButtonHover" );
	m_pStartWaveButton->setProperty( "PushedImage", "set:Essentials image:StartWaveButtonHover" );

	m_pMainWindow->addChildWindow(m_pStartWaveButton);
	m_pStartWaveButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&InGameHUD::StartWave, InGameHUD::GetSingletonPtr()));
	m_pStartWaveButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&InGameHUD::StartWaveHover, InGameHUD::GetSingletonPtr()));

	// Create enemy progress bar
	EnemyBar::GetSingletonPtr()->Init();

	m_pMainWindow->addChildWindow(EnemyBar::GetSingletonPtr()->GetMainWindowPtr());

	// Create coin icon with its stats 
	CEGUI::Window* coinIcon = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "HUD_CoinIcon");
	coinIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.04,0), CEGUI::UDim(1,0)));
	coinIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.6,0), CEGUI::UDim(0,0)));
	coinIcon->setProperty( "Image", "set:Essentials image:CoinIcon" );

	m_pMainWindow->addChildWindow(coinIcon);

	CEGUI::Window* coinStats = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText", "HUD_CoinStats");
	coinStats->setSize(CEGUI::UVector2(CEGUI::UDim(0.04,0), CEGUI::UDim(1,0)));
	coinStats->setPosition(CEGUI::UVector2(CEGUI::UDim(0.65,0), CEGUI::UDim(0,0)));
	coinStats->setText("0");

	m_pMainWindow->addChildWindow(coinStats);

	// Create wood icon with its stats 
	CEGUI::Window* woodIcon = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "HUD_WoodIcon");
	woodIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.04,0), CEGUI::UDim(1,0)));
	woodIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.7,0), CEGUI::UDim(0,0)));
	woodIcon->setProperty( "Image", "set:Essentials image:WoodIcon" );

	m_pMainWindow->addChildWindow(woodIcon);

	CEGUI::Window* woodStats = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText", "HUD_WoodStats");
	woodStats->setSize(CEGUI::UVector2(CEGUI::UDim(0.04,0), CEGUI::UDim(1,0)));
	woodStats->setPosition(CEGUI::UVector2(CEGUI::UDim(0.75,0), CEGUI::UDim(0,0)));
	woodStats->setText("0");

	m_pMainWindow->addChildWindow(woodStats);

	// Create stone icon with its stats 
	CEGUI::Window* stoneIcon = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "HUD_StoneIcon");
	stoneIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.04,0), CEGUI::UDim(1,0)));
	stoneIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.8,0), CEGUI::UDim(0,0)));
	stoneIcon->setProperty( "Image", "set:Essentials image:StoneIcon" );

	m_pMainWindow->addChildWindow(stoneIcon);

	CEGUI::Window* stoneStats = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText", "HUD_StoneStats");
	stoneStats->setSize(CEGUI::UVector2(CEGUI::UDim(0.04,0), CEGUI::UDim(1,0)));
	stoneStats->setPosition(CEGUI::UVector2(CEGUI::UDim(0.85,0), CEGUI::UDim(0,0)));
	stoneStats->setText("0");

	m_pMainWindow->addChildWindow(stoneStats);

	// Create iron icon with its stats 
	CEGUI::Window* ironIcon = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "HUD_IronIcon");
	ironIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.04,0), CEGUI::UDim(1,0)));
	ironIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.9,0), CEGUI::UDim(0,0)));
	ironIcon->setProperty( "Image", "set:Essentials image:IronIcon" );

	m_pMainWindow->addChildWindow(ironIcon);

	CEGUI::Window* ironStats = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText", "HUD_IronStats");
	ironStats->setSize(CEGUI::UVector2(CEGUI::UDim(0.04,0), CEGUI::UDim(1,0)));
	ironStats->setPosition(CEGUI::UVector2(CEGUI::UDim(0.95,0), CEGUI::UDim(0,0)));
	ironStats->setText("0");

	m_pMainWindow->addChildWindow(ironStats);

	CEGUI::System::getSingleton().getGUISheet()->addChildWindow(m_pMainWindow);

	// Create animation definition for hover a button
	m_pButtonHoverAnim = CEGUI::AnimationManager::getSingleton().createAnimation("HUD_ButtonHover");
	m_pButtonHoverAnim->setDuration(3.0f);
	m_pButtonHoverAnim->setReplayMode(CEGUI::Animation::RM_Once);

	CEGUI::Affector* affector = m_pButtonHoverAnim->createAffector("XRotation", "float");
	affector->createKeyFrame(0.0f, "0.0");
	affector->createKeyFrame(3.0f, "360.0");

	// Bind animation to windows
	m_pPauseButtonAnim = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pButtonHoverAnim);
	m_pPauseButtonAnim->setTargetWindow(m_pPauseButton);
	m_pSlowDownButtonAnim = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pButtonHoverAnim);
	m_pSlowDownButtonAnim->setTargetWindow(m_pSlowDownButton);
	m_pSpeedUpButtonAnim = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pButtonHoverAnim);
	m_pSpeedUpButtonAnim->setTargetWindow(m_pSpeedUpButton);
	m_pStartWaveButtonAnim = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pButtonHoverAnim);
	m_pStartWaveButtonAnim->setTargetWindow(m_pStartWaveButton);

	m_bInitialized = true;

	PushToBack();
}

void InGameHUD::MoveToTop()
{
	if(!m_bInitialized || m_bIsShown)
		return;

	m_pMainWindow->setAlwaysOnTop(true);
	m_pMainWindow->enable();
	m_pMainWindow->setVisible(true);

	m_pVillagerCounter->setAlwaysOnTop(true);
	m_pVillagerCounter->enable();
	m_pVillagerCounter->setVisible(true);
	
	m_bIsShown = true;
}

void InGameHUD::PushToBack()
{
	if(!m_bInitialized || !m_bIsShown)
		return;

	m_pMainWindow->setAlwaysOnTop(false);
	m_pMainWindow->disable();
	m_pMainWindow->setVisible(false);

	m_pVillagerCounter->setAlwaysOnTop(false);
	m_pVillagerCounter->disable();
	m_pVillagerCounter->setVisible(false);
	
	m_bIsShown = false;
}

void InGameHUD::PauseGame()
{
	UIManager::SwitchWindows("PauseMenu");

	if(dynamic_cast<Hills::TerrainTestState*>(Hills::StateManager::GetCurrentState()) != 0)
		return;

	TileMenu::GetSingletonPtr()->PushToBack();
	TowerMenu::GetSingletonPtr()->PushToBack();

    EnemyBar::GetSingletonPtr()->Pause();

    SoundManager::PauseAllSounds();
    SoundManager::LowerMusicVolume();

    (dynamic_cast<Hills::GameState*>(Hills::StateManager::GetCurrentState()) != 0)?
		dynamic_cast<Hills::GameState*>(Hills::StateManager::GetCurrentState())->SetPause() :
		dynamic_cast<Hills::PrototypeState*>(Hills::StateManager::GetCurrentState())->SetPause();
}

//-------------------------------------------------------------------------------------------
//	Function used when start wave. (By Pierre)
//-------------------------------------------------------------------------------------------
bool InGameHUD::StartWave(const CEGUI::EventArgs &p_eEventArgs)
{
	return true;
}

//-------------------------------------------------------------------------------------------
//	Function used when pause the game. (By Pierre)
//-------------------------------------------------------------------------------------------
bool InGameHUD::Pause(const CEGUI::EventArgs &p_eEventArgs)
{
	PauseGame();

	return true;
}

//-------------------------------------------------------------------------------------------
//	Function used when slow down the speed of game. (By Pierre)
//-------------------------------------------------------------------------------------------
bool InGameHUD::SlowDown(const CEGUI::EventArgs &p_eEventArgs)
{
    BaseApplication::SetGameSpeed(1.0f);
    EnemyBar::GetSingletonPtr()->SlowDown();

	return true;
}

//-------------------------------------------------------------------------------------------
//	Function used when speed up the game. (By Pierre)
//-------------------------------------------------------------------------------------------
bool InGameHUD::SpeedUp(const CEGUI::EventArgs &p_eEventArgs)
{
    BaseApplication::SetGameSpeed(2.0f);
    EnemyBar::GetSingletonPtr()->SpeedUp();

	return true;
}

bool InGameHUD::InGameHUDHover(const CEGUI::EventArgs &p_eEventArgs)
{
	SelectableManager::SetPause();

	return true;
}

bool InGameHUD::InGameHUDLeaves(const CEGUI::EventArgs &p_eEventArgs)
{
	SelectableManager::SetPause(false);

	return true;
}

//-------------------------------------------------------------------------------------------
//	Function used when start wave button be hovered. (By Pierre)
//-------------------------------------------------------------------------------------------
bool InGameHUD::StartWaveHover(const CEGUI::EventArgs &p_eEventArgs)
{
	m_pStartWaveButtonAnim->start();

	return true;
}

//-------------------------------------------------------------------------------------------
//	Function used when pause button be hovered (By Pierre)
//-------------------------------------------------------------------------------------------
bool InGameHUD::PauseHover(const CEGUI::EventArgs &p_eEventArgs)
{
	m_pPauseButtonAnim->start();

	return true;
}

//-------------------------------------------------------------------------------------------
//	Function used when slow down button be hovered (By Pierre)
//-------------------------------------------------------------------------------------------
bool InGameHUD::SlowDownHover(const CEGUI::EventArgs &p_eEventArgs)
{
	m_pSlowDownButtonAnim->start();

	return true;
}

//-------------------------------------------------------------------------------------------
//	Function used when speed up button be hovered (By Pierre)
//-------------------------------------------------------------------------------------------
bool InGameHUD::SpeedUpHover(const CEGUI::EventArgs &p_eEventArgs)
{
	m_pSpeedUpButtonAnim->start();

	return true;
}

void InGameHUD::SetCoinStats(int p_iAmount)
{
	CEGUI::WindowManager::getSingleton().getWindow("HUD_CoinStats")->
		setText(static_cast<std::ostringstream*>( &(std::ostringstream() << p_iAmount) )->str());
}

void InGameHUD::SetWoodStats(int p_iAmount)
{
	CEGUI::WindowManager::getSingleton().getWindow("HUD_WoodStats")->
		setText(static_cast<std::ostringstream*>( &(std::ostringstream() << p_iAmount) )->str());
}

void InGameHUD::SetStoneStats(int p_iAmount)
{
	CEGUI::WindowManager::getSingleton().getWindow("HUD_StoneStats")->
		setText(static_cast<std::ostringstream*>( &(std::ostringstream() << p_iAmount) )->str());
}

void InGameHUD::SetIronStats(int p_iAmount)
{
	CEGUI::WindowManager::getSingleton().getWindow("HUD_IronStats")->
		setText(static_cast<std::ostringstream*>( &(std::ostringstream() << p_iAmount) )->str());
}

void InGameHUD::IncreaseCoinStats()
{
	CEGUI::WindowManager::getSingleton().getWindow("HUD_CoinStats")->
		setText(static_cast<std::ostringstream*>( &(std::ostringstream() << ++m_iCoinStats) )->str());
}

void InGameHUD::IncreaseWoodStats()
{
	CEGUI::WindowManager::getSingleton().getWindow("HUD_WoodStats")->
		setText(static_cast<std::ostringstream*>( &(std::ostringstream() << ++m_iWoodStats) )->str());
}

void InGameHUD::IncreaseStoneStats()
{
	CEGUI::WindowManager::getSingleton().getWindow("HUD_StoneStats")->
		setText(static_cast<std::ostringstream*>( &(std::ostringstream() << ++m_iStoneStats) )->str());
}

void InGameHUD::IncreaseIronStats()
{
	CEGUI::WindowManager::getSingleton().getWindow("HUD_IronStats")->
		setText(static_cast<std::ostringstream*>( &(std::ostringstream() << ++m_iIronStats) )->str());
}