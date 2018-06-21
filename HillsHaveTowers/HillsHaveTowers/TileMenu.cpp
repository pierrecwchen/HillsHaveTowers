//-------------------------------------------------------------------------------------------
//	TileMenu.cpp											Original Author: Pierre Chen
//
//	Please see notes from TileMenu.h
//-------------------------------------------------------------------------------------------

#include "TileMenu.h"
#include "Player.h"
#include "SelectableManager.h"
#include "SoundManager.h"
#include <iostream>

//#define TMButtonCmp(x, y) ( ( ((x) & 0xff) & ((y) & 0xff) ) > 0 )

TileMenu* TileMenu::m_uiTileMenu = 0;

//-------------------------------------------------------------------------------------------
//	Private default constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
TileMenu::TileMenu(): m_fpWindowPos(0.0,0.0)
{
	m_pPlayer = 0;
	m_pTile = 0;
	m_pTileMenuMoveAnim = 0;
}

//-------------------------------------------------------------------------------------------
//	Private copy constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
TileMenu::TileMenu(const TileMenu& p_uiTileMenu)
{
}

//-------------------------------------------------------------------------------------------
//	Private destructor. (By Pierre)
//-------------------------------------------------------------------------------------------
TileMenu::~TileMenu()
{
	CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(m_pTileMenuShowAnim);
	CEGUI::AnimationManager::getSingleton().destroyAnimation(m_pTileMenuShowAnim);
	CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(m_pTileMenuMoveAnim);
	CEGUI::AnimationManager::getSingleton().destroyAnimation(m_pTileMenuMoveAnim);

	m_pPlayer = 0;
	m_pTile = 0;
	m_pMainWindow = 0;
	m_wArrowTowerButton = 0;
	m_wCannonTowerButton = 0;
	m_wSniperTowerButton = 0;
	m_wSlowTowerButton = 0;
	m_wFireTowerButton = 0;
	m_wCancelButton = 0;
}

//-------------------------------------------------------------------------------------------
//	Private assignment operator. (By Pierre)
//-------------------------------------------------------------------------------------------
void TileMenu::operator=(const TileMenu& p_uiTileMenu)
{
}

void TileMenu::CalculatePosition(const OIS::MouseEvent& pArg)
{
	m_fpWindowPos.first = CEGUI::MouseCursor::getSingletonPtr()->getPosition().d_x / float(pArg.state.width) - 0.1f;

	if(m_fpWindowPos.first < 0.0f)
		m_fpWindowPos.first = 0.0f;
	if(m_fpWindowPos.first > 0.8f)
		m_fpWindowPos.first = 0.8f;

	m_fpWindowPos.second = CEGUI::MouseCursor::getSingletonPtr()->getPosition().d_y / float(pArg.state.height) - 0.15f;

	if(m_fpWindowPos.second < 0.0f)
		m_fpWindowPos.second = 0.0f;
	if(m_fpWindowPos.second > 0.7f)
		m_fpWindowPos.second = 0.7f;
}

std::string TileMenu::TranslatePositionToString(const float p_fXPos, const float p_fYPos)
{
	// Format: {{p_fXPos,0.0},{p_fYPos,0.0}}
	std::string tempStr("{{");
	tempStr.append(static_cast<std::ostringstream*>( &(std::ostringstream() << p_fXPos) )->str());
	tempStr.append(",0.0},{");
	tempStr.append(static_cast<std::ostringstream*>( &(std::ostringstream() << p_fYPos) )->str());
	tempStr.append(",0.0}}");

	return tempStr;
}

//-------------------------------------------------------------------------------------------
//	Return singleton pointer. (By Pierre)
//-------------------------------------------------------------------------------------------
TileMenu* TileMenu::GetSingletonPtr()
{
	if(!m_uiTileMenu)
		m_uiTileMenu = new TileMenu();

	return m_uiTileMenu;
}

//-------------------------------------------------------------------------------------------
//	Public destructor. (By Pierre)
//-------------------------------------------------------------------------------------------
void TileMenu::Destroy()
{
	delete m_uiTileMenu;
}

//-------------------------------------------------------------------------------------------
//	Initialize the in-game HUD. (By Pierre)
//-------------------------------------------------------------------------------------------
void TileMenu::Init()
{
	if(m_bInitialized)
		return;

	m_tmButtons = TileMenuButton();

	m_pMainWindow = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "TileMenu");
	m_pMainWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.4,0)));
	m_pMainWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));

	// Create arrow tower button
	/*m_wArrowTowerButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TM_Arrow");
	m_wArrowTowerButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.38,0), CEGUI::UDim(0.38,0)));
	m_wArrowTowerButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0,0)));
	m_wArrowTowerButton->setProperty( "NormalImage", "set:ButtonBackground image:BlueBackground" );
	m_wArrowTowerButton->setProperty( "HoverImage", "set:ButtonBackground image:BlueBackground" );
	m_wArrowTowerButton->setProperty( "PushedImage", "set:ButtonBackground image:BlueBackground" );*/

	m_wArrowTowerButton = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_Arrow");
	m_wArrowTowerButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.32,0), CEGUI::UDim(0.32,0)));
	m_wArrowTowerButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0,0)));
	m_wArrowTowerButton->setProperty( "Image", "set:ButtonBackground image:BlueBackground" );

	CEGUI::Window* shooterIcon = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TM_Shooter");
	shooterIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.6,0), CEGUI::UDim(0.7,0)));
	shooterIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.15,0)));
	shooterIcon->setProperty( "NormalImage", "set:ArrowN image:full_image" );
	shooterIcon->setProperty( "HoverImage", "set:ArrowH image:full_image" );
	shooterIcon->setProperty( "PushedImage", "set:ArrowH image:full_image" );
	shooterIcon->setVisible(true);
	/*shooterIcon->setTooltipText("Gold: "+static_cast<std::ostringstream*>( &(std::ostringstream() << ArrowTower::GetCost(0).GetGold()) )->str()+"\n"+
								"Wood: "+static_cast<std::ostringstream*>( &(std::ostringstream() << ArrowTower::GetCost(0).GetWood()) )->str()+"\n"+
								"Stone: "+static_cast<std::ostringstream*>( &(std::ostringstream() << ArrowTower::GetCost(0).GetStone()) )->str()+"\n"+
								"Iron: "+static_cast<std::ostringstream*>( &(std::ostringstream() << ArrowTower::GetCost(0).GetIron()) )->str()+"\n");*/
	//shooterIcon->setAlpha(0.7);
	m_wArrowTowerButton->addChildWindow(shooterIcon);
	shooterIcon->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TileMenu::ArrowTower, TileMenu::GetSingletonPtr()));
	shooterIcon->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TileMenu::ArrowTowerHover, TileMenu::GetSingletonPtr()));
	shooterIcon->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TileMenu::ArrowTowerLeaves, TileMenu::GetSingletonPtr()));

	/*CEGUI::Window* stopText = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticText", "TM_ArrowT");
	stopText->setSize(CEGUI::UVector2(CEGUI::UDim(1.0,0), CEGUI::UDim(0.5,0)));
	stopText->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.25,0)));
	stopText->setText("Not enough\nresource\nto build");
	stopText->setFont("DejaVuSans-5");
	stopText->setProperty("TextColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000");
	stopText->setProperty("HorzFormatting", "HorzCentred");
	stopText->setProperty("VertFormatting", "VertCentred");*/

	CEGUI::Window* stopIcon = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_ArrowS");
	stopIcon->setSize(CEGUI::UVector2(CEGUI::UDim(1.0,0), CEGUI::UDim(1.0,0)));
	stopIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	stopIcon->setProperty("Image", "set:StopIcon image:full_image");
	stopIcon->setVisible(false);
	//stopIcon->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TileMenu::ArrowTowerHover, TileMenu::GetSingletonPtr()));

	//stopIcon->addChildWindow(stopText);
	m_wArrowTowerButton->addChildWindow(stopIcon);
	stopIcon->moveInFront(shooterIcon);

	/*CEGUI::Window* costIcon = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_ArrowGoldIcon");
	costIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.15,0), CEGUI::UDim(0.15,0)));
	costIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.2,0)));
	costIcon->setProperty("Image", "set:Essentials image:CoinIcon");
	m_wArrowTowerButton->addChildWindow(costIcon);
	costIcon->moveBehind(shooterIcon);

	CEGUI::Window* costText = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticText", "TM_ArrowGoldText");
	costText->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.15,0)));
	costText->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5,0), CEGUI::UDim(0.2,0)));
	costText->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << ArrowTower::GetCost(0).GetGold()) )->str());
	costText->setFont("DejaVuSans-5");
	costText->setProperty("TextColours", "tl:FF000000 tr:FF000000 bl:FF000000 br:FF000000");
	m_wArrowTowerButton->addChildWindow(costText);
	costText->moveBehind(shooterIcon);*/

	m_pMainWindow->addChildWindow(m_wArrowTowerButton);
	//m_wArrowTowerButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TileMenu::ArrowTower, TileMenu::GetSingletonPtr()));
	//m_wArrowTowerButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TileMenu::ArrowTowerHover, TileMenu::GetSingletonPtr()));
	//m_wArrowTowerButton->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TileMenu::ArrowTowerLeaves, TileMenu::GetSingletonPtr()));

	// Create cannon tower button
	/*m_wCannonTowerButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TM_Cannon");
	m_wCannonTowerButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.38,0), CEGUI::UDim(0.38,0)));
	m_wCannonTowerButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.25,0)));
	m_wCannonTowerButton->setProperty( "NormalImage", "set:ButtonBackground image:BlueBackground" );
	m_wCannonTowerButton->setProperty( "HoverImage", "set:ButtonBackground image:BlueBackground" );
	m_wCannonTowerButton->setProperty( "PushedImage", "set:ButtonBackground image:BlueBackground" );*/

	m_wCannonTowerButton = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_Cannon");
	m_wCannonTowerButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.32,0), CEGUI::UDim(0.32,0)));
	m_wCannonTowerButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.2,0)));
	m_wCannonTowerButton->setProperty( "Image", "set:ButtonBackground image:BlueBackground" );

	CEGUI::Window* cannonIcon = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TM_CannonIcon");
	cannonIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.6,0), CEGUI::UDim(0.7,0)));
	cannonIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.15,0)));
	cannonIcon->setProperty( "NormalImage", "set:CannonN image:full_image" );
	cannonIcon->setProperty( "HoverImage", "set:CannonH image:full_image" );
	cannonIcon->setProperty( "PushedImage", "set:CannonH image:full_image" );
	cannonIcon->setVisible(true);
	m_wCannonTowerButton->addChildWindow(cannonIcon);
	cannonIcon->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TileMenu::CannonTower, TileMenu::GetSingletonPtr()));
	cannonIcon->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TileMenu::CannonTowerHover, TileMenu::GetSingletonPtr()));
	cannonIcon->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TileMenu::CannonTowerLeaves, TileMenu::GetSingletonPtr()));

	/*stopText = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticText", "TM_CannonT");
	stopText->setSize(CEGUI::UVector2(CEGUI::UDim(1.0,0), CEGUI::UDim(0.5,0)));
	stopText->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.25,0)));
	stopText->setText("Not enough\nresource\nto build");
	stopText->setFont("DejaVuSans-5");
	stopText->setProperty("TextColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000");
	stopText->setProperty("HorzFormatting", "HorzCentred");
	stopText->setProperty("VertFormatting", "VertCentred");*/

	stopIcon = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_CannonS");
	stopIcon->setSize(CEGUI::UVector2(CEGUI::UDim(1.0,0), CEGUI::UDim(1.0,0)));
	stopIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	stopIcon->setProperty("Image", "set:StopIcon image:full_image");
	stopIcon->setVisible(false);

	//stopIcon->addChildWindow(stopText);
	m_wCannonTowerButton->addChildWindow(stopIcon);
	stopIcon->moveInFront(cannonIcon);

	m_pMainWindow->addChildWindow(m_wCannonTowerButton);
	//m_wCannonTowerButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TileMenu::CannonTower, TileMenu::GetSingletonPtr()));
	//m_wCannonTowerButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TileMenu::CannonTowerHover, TileMenu::GetSingletonPtr()));
	//m_wCannonTowerButton->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TileMenu::CannonTowerLeaves, TileMenu::GetSingletonPtr()));

	// Create fire tower button
	/*m_wFireTowerButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TM_Fire");
	m_wFireTowerButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.38,0), CEGUI::UDim(0.38,0)));
	m_wFireTowerButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.6,0), CEGUI::UDim(0.25,0)));
	m_wFireTowerButton->setProperty( "NormalImage", "set:ButtonBackground image:BlueBackground" );
	m_wFireTowerButton->setProperty( "HoverImage", "set:ButtonBackground image:BlueBackground" );
	m_wFireTowerButton->setProperty( "PushedImage", "set:ButtonBackground image:BlueBackground" );*/

	m_wFireTowerButton = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_Fire");
	m_wFireTowerButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.32,0), CEGUI::UDim(0.32,0)));
	m_wFireTowerButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.6,0), CEGUI::UDim(0.2,0)));
	m_wFireTowerButton->setProperty( "Image", "set:ButtonBackground image:BlueBackground" );

	CEGUI::Window* fireIcon = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TM_FireIcon");
	fireIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.6,0), CEGUI::UDim(0.7,0)));
	fireIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.15,0)));
	fireIcon->setProperty( "NormalImage", "set:FireIcon image:FireN" );
	fireIcon->setProperty( "HoverImage", "set:FireIcon image:FireH" );
	fireIcon->setProperty( "PushedImage", "set:FireIcon image:FireH" );
	fireIcon->setVisible(true);
	m_wFireTowerButton->addChildWindow(fireIcon);
	fireIcon->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TileMenu::FireTower, TileMenu::GetSingletonPtr()));
	fireIcon->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TileMenu::FireTowerHover, TileMenu::GetSingletonPtr()));
	fireIcon->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TileMenu::FireTowerLeaves, TileMenu::GetSingletonPtr()));
	
	/*stopText = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticText", "TM_FireT");
	stopText->setSize(CEGUI::UVector2(CEGUI::UDim(1.0,0), CEGUI::UDim(0.5,0)));
	stopText->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.25,0)));
	stopText->setText("Not enough\nresource\nto build");
	stopText->setFont("DejaVuSans-5");
	stopText->setProperty("TextColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000");
	stopText->setProperty("HorzFormatting", "HorzCentred");
	stopText->setProperty("VertFormatting", "VertCentred");*/

	stopIcon = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_FireS");
	stopIcon->setSize(CEGUI::UVector2(CEGUI::UDim(1.0,0), CEGUI::UDim(1.0,0)));
	stopIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	stopIcon->setProperty("Image", "set:StopIcon image:full_image");
	stopIcon->setVisible(false);

	//stopIcon->addChildWindow(stopText);
	m_wFireTowerButton->addChildWindow(stopIcon);
	stopIcon->moveInFront(fireIcon);

	m_pMainWindow->addChildWindow(m_wFireTowerButton);
	//m_wFireTowerButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TileMenu::FireTower, TileMenu::GetSingletonPtr()));
	//m_wFireTowerButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TileMenu::FireTowerHover, TileMenu::GetSingletonPtr()));
	//m_wFireTowerButton->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TileMenu::FireTowerLeaves, TileMenu::GetSingletonPtr()));

	// Create sniper tower button
	/*m_wSniperTowerButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TM_Sniper");
	m_wSniperTowerButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.38,0), CEGUI::UDim(0.38,0)));
	m_wSniperTowerButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.12,0), CEGUI::UDim(0.62,0)));
	m_wSniperTowerButton->setProperty( "NormalImage", "set:ButtonBackground image:BlueBackground" );
	m_wSniperTowerButton->setProperty( "HoverImage", "set:ButtonBackground image:BlueBackground" );
	m_wSniperTowerButton->setProperty( "PushedImage", "set:ButtonBackground image:BlueBackground" );*/

	m_wSniperTowerButton = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_Sniper");
	m_wSniperTowerButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.32,0), CEGUI::UDim(0.32,0)));
	m_wSniperTowerButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15,0), CEGUI::UDim(0.5,0)));
	m_wSniperTowerButton->setProperty( "Image", "set:ButtonBackground image:BlueBackground" );
	
	CEGUI::Window* sniperIcon = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TM_SniperIcon");
	sniperIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.6,0), CEGUI::UDim(0.7,0)));
	sniperIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.15,0)));
	sniperIcon->setProperty( "NormalImage", "set:SniperN image:full_image" );
	sniperIcon->setProperty( "HoverImage", "set:SniperH image:full_image" );
	sniperIcon->setProperty( "PushedImage", "set:SniperH image:full_image" );
	sniperIcon->setVisible(true);
	m_wSniperTowerButton->addChildWindow(sniperIcon);
	sniperIcon->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TileMenu::SniperTower, TileMenu::GetSingletonPtr()));
	sniperIcon->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TileMenu::SniperTowerHover, TileMenu::GetSingletonPtr()));
	sniperIcon->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TileMenu::SniperTowerLeaves, TileMenu::GetSingletonPtr()));

	/*stopText = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticText", "TM_SniperT");
	stopText->setSize(CEGUI::UVector2(CEGUI::UDim(1.0,0), CEGUI::UDim(0.5,0)));
	stopText->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.25,0)));
	stopText->setText("Not enough\nresource\nto build");
	stopText->setFont("DejaVuSans-5");
	stopText->setProperty("TextColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000");
	stopText->setProperty("HorzFormatting", "HorzCentred");
	stopText->setProperty("VertFormatting", "VertCentred");*/

	stopIcon = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_SniperS");
	stopIcon->setSize(CEGUI::UVector2(CEGUI::UDim(1.0,0), CEGUI::UDim(1.0,0)));
	stopIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	stopIcon->setProperty("Image", "set:StopIcon image:full_image");
	stopIcon->setVisible(false);

	//stopIcon->addChildWindow(stopText);
	m_wSniperTowerButton->addChildWindow(stopIcon);
	stopIcon->moveInFront(sniperIcon);

	m_pMainWindow->addChildWindow(m_wSniperTowerButton);
	//m_wSniperTowerButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TileMenu::SniperTower, TileMenu::GetSingletonPtr()));
	//m_wSniperTowerButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TileMenu::SniperTowerHover, TileMenu::GetSingletonPtr()));
	//m_wSniperTowerButton->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TileMenu::SniperTowerLeaves, TileMenu::GetSingletonPtr()));

	// Create slow tower button
	/*m_wSlowTowerButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TM_Slow");
	m_wSlowTowerButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.38,0), CEGUI::UDim(0.38,0)));
	m_wSlowTowerButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5,0), CEGUI::UDim(0.62,0)));
	m_wSlowTowerButton->setProperty( "NormalImage", "set:ButtonBackground image:BlueBackground" );
	m_wSlowTowerButton->setProperty( "HoverImage", "set:ButtonBackground image:BlueBackground" );
	m_wSlowTowerButton->setProperty( "PushedImage", "set:ButtonBackground image:BlueBackground" );*/

	m_wSlowTowerButton = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_Slow");
	m_wSlowTowerButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.32,0), CEGUI::UDim(0.32,0)));
	m_wSlowTowerButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.47,0), CEGUI::UDim(0.5,0)));
	m_wSlowTowerButton->setProperty( "Image", "set:ButtonBackground image:BlueBackground" );

	CEGUI::Window* slowIcon = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TM_SlowIcon");
	slowIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.6,0), CEGUI::UDim(0.7,0)));
	slowIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.15,0)));
	slowIcon->setProperty( "NormalImage", "set:SlowN image:full_image" );
	slowIcon->setProperty( "HoverImage", "set:SlowH image:full_image" );
	slowIcon->setProperty( "PushedImage", "set:SlowH image:full_image" );
	slowIcon->setVisible(true);
	m_wSlowTowerButton->addChildWindow(slowIcon);
	slowIcon->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TileMenu::SlowTower, TileMenu::GetSingletonPtr()));
	slowIcon->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TileMenu::SlowTowerHover, TileMenu::GetSingletonPtr()));
	slowIcon->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TileMenu::SlowTowerLeaves, TileMenu::GetSingletonPtr()));
	
	/*stopText = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticText", "TM_SlowT");
	stopText->setSize(CEGUI::UVector2(CEGUI::UDim(1.0,0), CEGUI::UDim(0.5,0)));
	stopText->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.25,0)));
	stopText->setText("Not enough\nresource\nto build");
	stopText->setFont("DejaVuSans-5");
	stopText->setProperty("TextColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000");
	stopText->setProperty("HorzFormatting", "HorzCentred");
	stopText->setProperty("VertFormatting", "VertCentred");*/

	stopIcon = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_SlowS");
	stopIcon->setSize(CEGUI::UVector2(CEGUI::UDim(1.0,0), CEGUI::UDim(1.0,0)));
	stopIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	stopIcon->setProperty("Image", "set:StopIcon image:full_image");
	stopIcon->setVisible(false);

	//stopIcon->addChildWindow(stopText);
	m_wSlowTowerButton->addChildWindow(stopIcon);
	stopIcon->moveInFront(slowIcon);

	m_pMainWindow->addChildWindow(m_wSlowTowerButton);
	//m_wSlowTowerButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TileMenu::SlowTower, TileMenu::GetSingletonPtr()));
	//m_wSlowTowerButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TileMenu::SlowTowerHover, TileMenu::GetSingletonPtr()));
	//m_wSlowTowerButton->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TileMenu::SlowTowerLeaves, TileMenu::GetSingletonPtr()));

	// Create cancel button
	m_wCancelButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TM_Cancel");
	m_wCancelButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.2,0)));
	m_wCancelButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.37,0), CEGUI::UDim(0.33,0)));
	m_wCancelButton->setProperty( "NormalImage", "set:Cancel image:full_image" );
	m_wCancelButton->setProperty( "HoverImage", "set:Cancel image:full_image" );
	m_wCancelButton->setProperty( "PushedImage", "set:Cancel image:full_image" );
	m_wCancelButton->setAlpha(0.6);

	m_pMainWindow->addChildWindow(m_wCancelButton);
	m_wCancelButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TileMenu::Cancel, TileMenu::GetSingletonPtr()));
	m_wCancelButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TileMenu::CancelHover, TileMenu::GetSingletonPtr()));
	m_wCancelButton->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TileMenu::CancelLeaves, TileMenu::GetSingletonPtr()));


	// Create cost spots
	// Background
	/*CEGUI::Window* costBarBackground = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_CostBarBack");
	costBarBackground->setSize(CEGUI::UVector2(CEGUI::UDim(0.95,0), CEGUI::UDim(0.18,0)));
	costBarBackground->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.82,0)));
	costBarBackground->setProperty("Image", "set:CostBarBackground image:full_image");
	m_pMainWindow->addChildWindow(costBarBackground);*/

	// Gold
	CEGUI::Window* costIcon = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_GoldIcon");
	costIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.1,0), CEGUI::UDim(0.1,0)));
	costIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.86,0)));
	costIcon->setProperty("Image", "set:Essentials image:CoinIcon");
	m_pMainWindow->addChildWindow(costIcon);

	CEGUI::Window* costText = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticText", "TM_GoldText");
	costText->setSize(CEGUI::UVector2(CEGUI::UDim(0.12,0), CEGUI::UDim(0.1,0)));
	costText->setPosition(CEGUI::UVector2(CEGUI::UDim(0.115,0), CEGUI::UDim(0.86,0)));
	costText->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	costText->setFont("DejaVuSans-5");
	costText->setProperty("TextColours", "tl:FF000000 tr:FF000000 bl:FF000000 br:FF000000");
	costText->setProperty("BackgroundEnabled", "true");
	m_pMainWindow->addChildWindow(costText);

	// Wood
	costIcon = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_WoodIcon");
	costIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.1,0), CEGUI::UDim(0.1,0)));
	costIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.25,0), CEGUI::UDim(0.86,0)));
	costIcon->setProperty("Image", "set:Essentials image:WoodIcon");
	m_pMainWindow->addChildWindow(costIcon);

	costText = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticText", "TM_WoodText");
	costText->setSize(CEGUI::UVector2(CEGUI::UDim(0.12,0), CEGUI::UDim(0.1,0)));
	costText->setPosition(CEGUI::UVector2(CEGUI::UDim(0.365,0), CEGUI::UDim(0.86,0)));
	costText->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	costText->setFont("DejaVuSans-5");
	costText->setProperty("TextColours", "tl:FF000000 tr:FF000000 bl:FF000000 br:FF000000");
	costText->setProperty("BackgroundEnabled", "true");
	m_pMainWindow->addChildWindow(costText);

	// Stone
	costIcon = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_StoneIcon");
	costIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.1,0), CEGUI::UDim(0.1,0)));
	costIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5,0), CEGUI::UDim(0.86,0)));
	costIcon->setProperty("Image", "set:Essentials image:StoneIcon");
	m_pMainWindow->addChildWindow(costIcon);

	costText = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticText", "TM_StoneText");
	costText->setSize(CEGUI::UVector2(CEGUI::UDim(0.12,0), CEGUI::UDim(0.1,0)));
	costText->setPosition(CEGUI::UVector2(CEGUI::UDim(0.615,0), CEGUI::UDim(0.86,0)));
	costText->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	costText->setFont("DejaVuSans-5");
	costText->setProperty("TextColours", "tl:FF000000 tr:FF000000 bl:FF000000 br:FF000000");
	costText->setProperty("BackgroundEnabled", "true");
	m_pMainWindow->addChildWindow(costText);

	// Iron
	costIcon = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TM_IronIcon");
	costIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.1,0), CEGUI::UDim(0.1,0)));
	costIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.75,0), CEGUI::UDim(0.86,0)));
	costIcon->setProperty("Image", "set:Essentials image:IronIcon");
	m_pMainWindow->addChildWindow(costIcon);

	costText = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticText", "TM_IronText");
	costText->setSize(CEGUI::UVector2(CEGUI::UDim(0.12,0), CEGUI::UDim(0.1,0)));
	costText->setPosition(CEGUI::UVector2(CEGUI::UDim(0.865,0), CEGUI::UDim(0.86,0)));
	costText->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	costText->setFont("DejaVuSans-5");
	costText->setProperty("TextColours", "tl:FF000000 tr:FF000000 bl:FF000000 br:FF000000");
	costText->setProperty("BackgroundEnabled", "true");
	m_pMainWindow->addChildWindow(costText);

	CEGUI::System::getSingleton().getGUISheet()->addChildWindow(m_pMainWindow);

	// Create menu show up animation
	m_pTileMenuShowAnim = CEGUI::AnimationManager::getSingleton().createAnimation("TM_Show");
	m_pTileMenuShowAnim->setDuration(0.3f);
	m_pTileMenuShowAnim->setReplayMode(CEGUI::Animation::RM_Once);

	CEGUI::Affector* affector = m_pTileMenuShowAnim->createAffector("UnifiedSize", "UVector2");
	affector->createKeyFrame(0.0f, "{{0.0, 0.0},{0.0,0.0}}");
	affector->createKeyFrame(0.3f, "{{0.2, 0.0},{0.3,0.0}}",CEGUI::KeyFrame::Progression::P_QuadraticDecelerating);

	affector = m_pTileMenuShowAnim->createAffector("UnifiedPosition", "UVector2");
	affector->createKeyFrame(0.0f);
	affector->createKeyFrame(0.3f);
	affector->getKeyFrameAtPosition(0.3f)->setProgression(CEGUI::KeyFrame::Progression::P_QuadraticDecelerating);

	m_pTileMenuShow = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pTileMenuShowAnim);
	m_pTileMenuShow->setTargetWindow(m_pMainWindow);

	// Create menu moving animation
	m_pTileMenuMoveAnim = CEGUI::AnimationManager::getSingleton().createAnimation("TM_Move");
	m_pTileMenuMoveAnim->setDuration(0.3f);
	m_pTileMenuMoveAnim->setReplayMode(CEGUI::Animation::RM_Once);

	affector = m_pTileMenuMoveAnim->createAffector("UnifiedPosition", "UVector2");
	affector->createKeyFrame(0.0f);
	affector->createKeyFrame(0.3f);
	affector->getKeyFrameAtPosition(0.3f)->setProgression(CEGUI::KeyFrame::Progression::P_QuadraticDecelerating);

	m_pTileMenuMove = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pTileMenuMoveAnim);
	m_pTileMenuMove->setTargetWindow(m_pMainWindow);

	// Hide animation, use the same pause menu fade out animation
	m_pTileMenuHideAnim = CEGUI::AnimationManager::getSingleton().getAnimation("PM_FadeOut");

	m_pTileMenuHide = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pTileMenuHideAnim);
	m_pTileMenuHide->setTargetWindow(m_pMainWindow);

	m_bInitialized = true;

	BaseWindow::PushToBack();
}

void TileMenu::MoveToTop()
{
	BaseWindow::MoveToTop();

	if(m_pTileMenuHide->isRunning())
		m_pTileMenuHide->stop();

	if(m_pMainWindow->getAlpha() != 1.0)
		m_pMainWindow->setAlpha(1.0f);

	m_pTileMenuShowAnim->getAffectorAtIdx(1)->getKeyFrameAtPosition(0.0f)->setValue(TranslatePositionToString(m_fpWindowPos.first + 0.1f, m_fpWindowPos.second + 0.15f).c_str());
	m_pTileMenuShowAnim->getAffectorAtIdx(1)->getKeyFrameAtPosition(0.3f)->setValue(TranslatePositionToString(m_fpWindowPos.first, m_fpWindowPos.second).c_str());

	m_pTileMenuShow->start();
}

void TileMenu::PushToBack()
{
	if(!m_bInitialized || !m_bIsShown)
		return;

	m_pTileMenuHide->start();

	m_bIsShown = false;
}

void TileMenu::MoveMenuTo(const OIS::MouseEvent& pArg)
{
	if(!m_bInitialized)
		return;

	if(!m_pTile)
		PushToBack();

	else
	{
		CalculatePosition(pArg);
		EnableButton();

		if(!m_bIsShown)
		{
			m_pMainWindow->setPosition(CEGUI::UVector2(
				CEGUI::UDim(m_fpWindowPos.first,0.0f),
				CEGUI::UDim(m_fpWindowPos.second,0.0f)));
			
			MoveToTop();
		}

		else
		{
			/*if(m_pTileMenuMoveAnim)
			{
				CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(m_pTileMenuMoveAnim);
				CEGUI::AnimationManager::getSingleton().destroyAnimation(m_pTileMenuMoveAnim);
			}
			
			m_pTileMenuMoveAnim = CEGUI::AnimationManager::getSingleton().createAnimation("TM_Move");
			m_pTileMenuMoveAnim->setDuration(0.3f);
			m_pTileMenuMoveAnim->setReplayMode(CEGUI::Animation::RM_Once);
			
			CEGUI::Affector* affector = m_pTileMenuMoveAnim->createAffector("UnifiedPosition", "UVector2");
			affector->createKeyFrame(0.0f, TranslatePositionToString(m_pMainWindow->getPosition().d_x.d_scale, m_pMainWindow->getPosition().d_y.d_scale).c_str());
			affector->createKeyFrame(0.3f, TranslatePositionToString(m_fpWindowPos.first, m_fpWindowPos.second).c_str(), CEGUI::KeyFrame::Progression::P_QuadraticDecelerating);
			
			m_pTileMenuMove = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pTileMenuMoveAnim);
			m_pTileMenuMove->setTargetWindow(m_pMainWindow);
			m_pTileMenuMove->start();*/

			/*if(m_pTileMenuMoveAnim->getNumAffectors() != 0)
			{
				for(int i = 0; i < m_pTileMenuMoveAnim->getNumAffectors(); ++i)
				{
					m_pTileMenuMoveAnim->destroyAffector(m_pTileMenuMoveAnim->getAffectorAtIdx(i));
				}
			}*/

			/*CEGUI::Affector* affector = m_pTileMenuMoveAnim->createAffector("UnifiedPosition", "UVector2");
			
			affector->createKeyFrame(0.0f, 
				TranslatePositionToString(m_pMainWindow->getPosition().d_x.d_scale, m_pMainWindow->getPosition().d_y.d_scale));
			
			affector->createKeyFrame(0.3f,
				TranslatePositionToString(m_fpWindowPos.first, m_fpWindowPos.second),
				CEGUI::KeyFrame::Progression::P_QuadraticDecelerating);

			m_pTileMenuMove = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pTileMenuMoveAnim);
			m_pTileMenuMove->setTargetWindow(m_pMainWindow);
			m_pTileMenuMove->start();*/

			m_pTileMenuMoveAnim->getAffectorAtIdx(0)->getKeyFrameAtPosition(0.0f)->setValue(TranslatePositionToString(m_pMainWindow->getPosition().d_x.d_scale, m_pMainWindow->getPosition().d_y.d_scale).c_str());
			//m_pTileMenuMoveAnim->getAffectorAtIdx(0)->getKeyFrameAtPosition(0.0f)->setValue("{{0.0, 0.0},{0.0,0.0}}");
			m_pTileMenuMoveAnim->getAffectorAtIdx(0)->getKeyFrameAtPosition(0.3f)->setValue(TranslatePositionToString(m_fpWindowPos.first, m_fpWindowPos.second).c_str());
			m_pTileMenuMove->start();
		}
	}
}

void TileMenu::EnableButton()
{
	// Arrow
	(MainCurrency::CanAfford(ArrowTower::GetCost(0)))?
		CEGUI::WindowManager::getSingleton().getWindow("TM_ArrowS")->setVisible(false):
		CEGUI::WindowManager::getSingleton().getWindow("TM_ArrowS")->setVisible(true);

	// Cannon
	(MainCurrency::CanAfford(CannonTower::GetCost(0)))?
		CEGUI::WindowManager::getSingleton().getWindow("TM_CannonS")->setVisible(false):
		CEGUI::WindowManager::getSingleton().getWindow("TM_CannonS")->setVisible(true);

	// Fire
	(MainCurrency::CanAfford(BurstTower::GetCost(0)))?
		CEGUI::WindowManager::getSingleton().getWindow("TM_FireS")->setVisible(false):
		CEGUI::WindowManager::getSingleton().getWindow("TM_FireS")->setVisible(true);

	// Sniper
	(MainCurrency::CanAfford(SniperTower::GetCost(0)))?
		CEGUI::WindowManager::getSingleton().getWindow("TM_SniperS")->setVisible(false):
		CEGUI::WindowManager::getSingleton().getWindow("TM_SniperS")->setVisible(true);

	// Slow
	(MainCurrency::CanAfford(SlowTower::GetCost(0)))?
		CEGUI::WindowManager::getSingleton().getWindow("TM_SlowS")->setVisible(false):
		CEGUI::WindowManager::getSingleton().getWindow("TM_SlowS")->setVisible(true);
}

bool TileMenu::ArrowTowerHover(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonHover.wav");

	if(!m_tmButtons.m_bArrow)
		return true;

	float positionIndex = 0;

	if(ArrowTower::GetCost(0).GetGold() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << ArrowTower::GetCost(0).GetGold()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(true);
		positionIndex++;
	}

	if(ArrowTower::GetCost(0).GetWood() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << ArrowTower::GetCost(0).GetWood()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(true);
		positionIndex++;
	}

	if(ArrowTower::GetCost(0).GetStone() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << ArrowTower::GetCost(0).GetStone()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(true);
		positionIndex++;
	}

	if(ArrowTower::GetCost(0).GetIron() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << ArrowTower::GetCost(0).GetIron()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(true);
		positionIndex++;
	}

	SelectableManager::SetPause();

	return true;
}

bool TileMenu::CannonTowerHover(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonHover.wav");

	if(!m_tmButtons.m_bCannon)
		return true;

	float positionIndex = 0;

	if(CannonTower::GetCost(0).GetGold() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << CannonTower::GetCost(0).GetGold()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(true);
		positionIndex++;
	}

	if(CannonTower::GetCost(0).GetWood() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << CannonTower::GetCost(0).GetWood()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(true);
		positionIndex++;
	}

	if(CannonTower::GetCost(0).GetStone() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << CannonTower::GetCost(0).GetStone()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(true);
		positionIndex++;
	}

	if(CannonTower::GetCost(0).GetIron() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << CannonTower::GetCost(0).GetIron()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(true);
		positionIndex++;
	}

	SelectableManager::SetPause();

	return true;
}

bool TileMenu::SniperTowerHover(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonHover.wav");

	if(!m_tmButtons.m_bSniper)
		return true;

	float positionIndex = 0;

	if(SniperTower::GetCost(0).GetGold() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << SniperTower::GetCost(0).GetGold()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(true);
		positionIndex++;
	}

	if(SniperTower::GetCost(0).GetWood() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << SniperTower::GetCost(0).GetWood()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(true);
		positionIndex++;
	}

	if(SniperTower::GetCost(0).GetStone() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << SniperTower::GetCost(0).GetStone()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(true);
		positionIndex++;
	}

	if(SniperTower::GetCost(0).GetIron() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << SniperTower::GetCost(0).GetIron()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(true);
		positionIndex++;
	}

	SelectableManager::SetPause();

	return true;
}

bool TileMenu::FireTowerHover(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonHover.wav");

	if(!m_tmButtons.m_bFire)
		return true;

	float positionIndex = 0;

	if(BurstTower::GetCost(0).GetGold() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << BurstTower::GetCost(0).GetGold()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(true);
		positionIndex++;
	}

	if(BurstTower::GetCost(0).GetWood() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << BurstTower::GetCost(0).GetWood()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(true);
		positionIndex++;
	}

	if(BurstTower::GetCost(0).GetStone() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << BurstTower::GetCost(0).GetStone()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(true);
		positionIndex++;
	}

	if(BurstTower::GetCost(0).GetIron() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << BurstTower::GetCost(0).GetIron()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(true);
		positionIndex++;
	}

	SelectableManager::SetPause();

	return true;
}

bool TileMenu::SlowTowerHover(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonHover.wav");

	if(!m_tmButtons.m_bSlow)
		return true;

	float positionIndex = 0;

	if(SlowTower::GetCost(0).GetGold() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << SlowTower::GetCost(0).GetGold()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(true);
		positionIndex++;
	}

	if(SlowTower::GetCost(0).GetWood() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << SlowTower::GetCost(0).GetWood()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(true);
		positionIndex++;
	}

	if(SlowTower::GetCost(0).GetStone() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << SlowTower::GetCost(0).GetStone()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(true);
		positionIndex++;
	}

	if(SlowTower::GetCost(0).GetIron() == 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(false);
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(false);
	}

	else
	{
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setXPosition(CEGUI::UDim(0.25*positionIndex,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setXPosition(CEGUI::UDim(0.25*positionIndex+0.115,0));
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(true);
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << SlowTower::GetCost(0).GetIron()) )->str());
		CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(true);
		positionIndex++;
	}

	SelectableManager::SetPause();

	return true;
}

bool TileMenu::CancelHover(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonHover.wav");

	m_wCancelButton->setAlpha(1.0);

	/*CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());*/

	CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(false);

	SelectableManager::SetPause();

	return true;
}

bool TileMenu::ArrowTowerLeaves(const CEGUI::EventArgs &p_eEventArgs)
{
	if(!m_tmButtons.m_bArrow)
		return true;

	/*CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());*/

	CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(false);

	SelectableManager::SetPause(false);

	return true;
}

bool TileMenu::CannonTowerLeaves(const CEGUI::EventArgs &p_eEventArgs)
{
	if(!m_tmButtons.m_bCannon)
		return true;

	/*CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());*/

	CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(false);

	SelectableManager::SetPause(false);

	return true;
}

bool TileMenu::SniperTowerLeaves(const CEGUI::EventArgs &p_eEventArgs)
{
	if(!m_tmButtons.m_bSniper)
		return true;

	/*CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());*/

	CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(false);

	SelectableManager::SetPause(false);

	return true;
}

bool TileMenu::FireTowerLeaves(const CEGUI::EventArgs &p_eEventArgs)
{
	if(!m_tmButtons.m_bFire)
		return true;

	/*CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());*/

	CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(false);

	SelectableManager::SetPause(false);

	return true;
}

bool TileMenu::SlowTowerLeaves(const CEGUI::EventArgs &p_eEventArgs)
{
	if(!m_tmButtons.m_bSlow)
		return true;

	/*CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());*/

	CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(false);

	SelectableManager::SetPause(false);

	return true;
}

bool TileMenu::CancelLeaves(const CEGUI::EventArgs &p_eEventArgs)
{
	m_wCancelButton->setAlpha(0.6);

	/*CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setText(static_cast<std::ostringstream*>( &(std::ostringstream() << 0) )->str());*/

	CEGUI::WindowManager::getSingleton().getWindow("TM_GoldIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronIcon")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_GoldText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_WoodText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_StoneText")->setVisible(false);
	CEGUI::WindowManager::getSingleton().getWindow("TM_IronText")->setVisible(false);

	SelectableManager::SetPause(false);

	return true;
}

bool TileMenu::ArrowTower(const CEGUI::EventArgs &p_eEventArgs)
{
	if(!m_tmButtons.m_bArrow)
		return true;

    // If the player has enough resources, we deduct the proper amount and build the tower
    if (MainCurrency::CanAfford(ArrowTower::GetCost(0)))
    {
	    static_cast<Player*>(m_pPlayer)->SetTowerToBuild(TTB_ArrowTower);

	    if(static_cast<Tile*>(m_pTile) != 0)
	    {
            SoundManager::Play2DSound("menuButtonPress.wav");

		    static_cast<Player*>(m_pPlayer)->MoveTo(static_cast<Tile*>(m_pTile));
		    //static_cast<Player*>(m_pPlayer)->ToBuild(true);
		    //static_cast<Player*>(m_pPlayer)->ToHarvest(false);
		    //static_cast<Player*>(m_pPlayer)->ToUpgrade(false);  // reset all flags if player had a previous selection
            static_cast<Player*>(m_pPlayer)->SetImpendingAction(PA_BUILD);

		    m_pTile = 0;

		    PushToBack();
	    }
    }

	return true;
}

bool TileMenu::CannonTower(const CEGUI::EventArgs &p_eEventArgs)
{
	if(!m_tmButtons.m_bCannon)
		return true;

    // If the player has enough resources, we deduct the proper amount and build the tower
    if (MainCurrency::CanAfford(CannonTower::GetCost(0)))
    {
	    static_cast<Player*>(m_pPlayer)->SetTowerToBuild(TTB_CannonTower);

	    if(static_cast<Tile*>(m_pTile) != 0)
	    {
            SoundManager::Play2DSound("menuButtonPress.wav");

		    static_cast<Player*>(m_pPlayer)->MoveTo(static_cast<Tile*>(m_pTile));
		    //static_cast<Player*>(m_pPlayer)->ToBuild(true);
		    //static_cast<Player*>(m_pPlayer)->ToHarvest(false);
		    //static_cast<Player*>(m_pPlayer)->ToUpgrade(false);  // reset all flags if player had a previous selection
            static_cast<Player*>(m_pPlayer)->SetImpendingAction(PA_BUILD);

		    m_pTile = 0;

		    PushToBack();
	    }
    }

	return true;
}

bool TileMenu::SniperTower(const CEGUI::EventArgs &p_eEventArgs)
{
	if(!m_tmButtons.m_bSniper)
		return true;

    // If the player has enough resources, we deduct the proper amount and build the tower
    if (MainCurrency::CanAfford(SniperTower::GetCost(0)))
    {
	    static_cast<Player*>(m_pPlayer)->SetTowerToBuild(TTB_SniperTower);

	    if(static_cast<Tile*>(m_pTile) != 0)
	    {
            SoundManager::Play2DSound("menuButtonPress.wav");

		    static_cast<Player*>(m_pPlayer)->MoveTo(static_cast<Tile*>(m_pTile));
		    //static_cast<Player*>(m_pPlayer)->ToBuild(true);
		    //static_cast<Player*>(m_pPlayer)->ToHarvest(false);
		    //static_cast<Player*>(m_pPlayer)->ToUpgrade(false);  // reset all flags if player had a previous selection
            static_cast<Player*>(m_pPlayer)->SetImpendingAction(PA_BUILD);

		    m_pTile = 0;
		
		    PushToBack();
	    }
    }

	return true;
}

bool TileMenu::FireTower(const CEGUI::EventArgs &p_eEventArgs)
{
	if(!m_tmButtons.m_bFire)
		return true;

    // If the player has enough resources, we deduct the proper amount and build the tower
    if (MainCurrency::CanAfford(BurstTower::GetCost(0)))
    {
	    static_cast<Player*>(m_pPlayer)->SetTowerToBuild(TTB_BurstTower);

	    if(static_cast<Tile*>(m_pTile) != 0)
	    {
            SoundManager::Play2DSound("menuButtonPress.wav");

		    static_cast<Player*>(m_pPlayer)->MoveTo(static_cast<Tile*>(m_pTile));
		    //static_cast<Player*>(m_pPlayer)->ToBuild(true);
		    //static_cast<Player*>(m_pPlayer)->ToHarvest(false);
		    //static_cast<Player*>(m_pPlayer)->ToUpgrade(false);  // reset all flags if player had a previous selection
            static_cast<Player*>(m_pPlayer)->SetImpendingAction(PA_BUILD);

		    m_pTile = 0;
		
		    PushToBack();
	    }
    }

	return true;
}

bool TileMenu::SlowTower(const CEGUI::EventArgs &p_eEventArgs)
{
	if(!m_tmButtons.m_bSlow)
		return true;

    // If the player has enough resources, we deduct the proper amount and build the tower
    if (MainCurrency::CanAfford(SlowTower::GetCost(0)))
    {
	    static_cast<Player*>(m_pPlayer)->SetTowerToBuild(TTB_SlowTower);

	    if(static_cast<Tile*>(m_pTile) != 0)
	    {
            SoundManager::Play2DSound("menuButtonPress.wav");

		    static_cast<Player*>(m_pPlayer)->MoveTo(static_cast<Tile*>(m_pTile));
		    //static_cast<Player*>(m_pPlayer)->ToBuild(true);
		    //static_cast<Player*>(m_pPlayer)->ToHarvest(false);
		    //static_cast<Player*>(m_pPlayer)->ToUpgrade(false);  // reset all flags if player had a previous selection
            static_cast<Player*>(m_pPlayer)->SetImpendingAction(PA_BUILD);

		    m_pTile = 0;
		
		    PushToBack();
	    }
    }

	return true;
}

bool TileMenu::Cancel(const CEGUI::EventArgs &p_eEventArgs)
{
	m_pTile = 0;

	PushToBack();

	return true;
}