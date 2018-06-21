//-------------------------------------------------------------------------------------------
//	TowerMenu.cpp											Original Author: Pierre Chen
//
//	Please see notes from TileMenu.h
//-------------------------------------------------------------------------------------------

#include "TowerMenu.h"
#include "Player.h"
#include "BaseTower.h"
#include "SelectableManager.h"
#include "SoundManager.h"

TowerMenu* TowerMenu::m_uiTowerMenu = 0;

//-------------------------------------------------------------------------------------------
//	Private default constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
TowerMenu::TowerMenu(): m_bUpgradeButton(true), m_bMouseCursorChanged(false) , m_fpWindowPos(0.0, 0.0)
{
	m_pPlayer = 0;
	m_pTower = 0;
}

//-------------------------------------------------------------------------------------------
//	Private copy constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
TowerMenu::TowerMenu(const TowerMenu& p_uiTowerMenu)
{
}

//-------------------------------------------------------------------------------------------
//	Private destructor. (By Pierre)
//-------------------------------------------------------------------------------------------
TowerMenu::~TowerMenu()
{
	CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(m_pTowerMenuShowAnim);
	CEGUI::AnimationManager::getSingleton().destroyAnimation(m_pTowerMenuShowAnim);
	CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(m_pTowerMenuMoveAnim);
	CEGUI::AnimationManager::getSingleton().destroyAnimation(m_pTowerMenuMoveAnim);

	m_bMouseCursorChanged = false;

	m_pPlayer = 0;
	m_pTower = 0;
	m_pMainWindow = 0;
	m_wTargetButton = 0;
	m_wSalvageButton = 0;
	m_wUpgradeButton = 0;
}

//-------------------------------------------------------------------------------------------
//	Private assignment operator. (By Pierre)
//-------------------------------------------------------------------------------------------
void TowerMenu::operator=(const TowerMenu& p_uiTowerMenu)
{
}

void TowerMenu::CalculatePosition(const OIS::MouseEvent& pArg)
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

std::string TowerMenu::TranslatePositionToString(const float p_fXPos, const float p_fYPos)
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
TowerMenu* TowerMenu::GetSingletonPtr()
{
	if(!m_uiTowerMenu)
		m_uiTowerMenu = new TowerMenu();

	return m_uiTowerMenu;
}

//-------------------------------------------------------------------------------------------
//	Public destructor. (By Pierre)
//-------------------------------------------------------------------------------------------
void TowerMenu::Destroy()
{
	delete m_uiTowerMenu;
}

//-------------------------------------------------------------------------------------------
//	Initialize the in-game HUD. (By Pierre)
//-------------------------------------------------------------------------------------------
void TowerMenu::Init()
{
	if(m_bInitialized)
		return;

	m_pMainWindow = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "TowerMenu");
	m_pMainWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.3,0)));
	m_pMainWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));

	// Create target button
	/*m_wTargetButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TrM_Target");
	m_wTargetButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.4,0)));
	m_wTargetButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0,0)));
	m_wTargetButton->setProperty( "NormalImage", "set:Essentials image:InfoNormal" );
	m_wTargetButton->setProperty( "HoverImage", "set:Essentials image:InfoHover" );
	m_wTargetButton->setProperty( "PushedImage", "set:Essentials image:InfoHover" );*/

	m_wTargetButton = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TrM_Target");
	m_wTargetButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.4,0)));
	m_wTargetButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0,0)));
	m_wTargetButton->setProperty( "Image", "set:ButtonBackground image:BlueBackground" );

	CEGUI::Window* targetingIcon = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TrM_TargetIcon");
	targetingIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.6,0), CEGUI::UDim(0.7,0)));
	targetingIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.15,0)));
	targetingIcon->setProperty( "NormalImage", "set:TargetingN image:full_image" );
	targetingIcon->setProperty( "HoverImage", "set:TargetingH image:full_image" );
	targetingIcon->setProperty( "PushedImage", "set:TargetingH image:full_image" );
	targetingIcon->setVisible(true);
	m_wTargetButton->addChildWindow(targetingIcon);
	targetingIcon->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TowerMenu::Target, TowerMenu::GetSingletonPtr()));
	targetingIcon->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TowerMenu::TargetHover, TowerMenu::GetSingletonPtr()));
	targetingIcon->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TowerMenu::TargetLeaves, TowerMenu::GetSingletonPtr()));
	
	m_pMainWindow->addChildWindow(m_wTargetButton);
	//m_wTargetButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TowerMenu::Target, TowerMenu::GetSingletonPtr()));
	//m_wTargetButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TowerMenu::TargetHover, TowerMenu::GetSingletonPtr()));
	//m_wTargetButton->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TowerMenu::TargetLeaves, TowerMenu::GetSingletonPtr()));

	// Create salvage button
	/*m_wSalvageButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TrM_Salvage");
	m_wSalvageButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.4,0)));
	m_wSalvageButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.6,0)));
	m_wSalvageButton->setProperty( "NormalImage", "set:ButtonBackground image:BlueBackground" );
	m_wSalvageButton->setProperty( "HoverImage", "set:ButtonBackground image:BlueBackground" );
	m_wSalvageButton->setProperty( "PushedImage", "set:ButtonBackground image:BlueBackground" );*/

	m_wSalvageButton = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TrM_Salvage");
	m_wSalvageButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.4,0)));
	m_wSalvageButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.6,0)));
	m_wSalvageButton->setProperty( "Image", "set:ButtonBackground image:BlueBackground" );

	CEGUI::Window* salvageIcon = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TrM_SalvageIcon");
	salvageIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.6,0), CEGUI::UDim(0.7,0)));
	salvageIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.15,0)));
	salvageIcon->setProperty( "NormalImage", "set:SalvageN image:full_image" );
	salvageIcon->setProperty( "HoverImage", "set:SalvageH image:full_image" );
	salvageIcon->setProperty( "PushedImage", "set:SalvageH image:full_image" );
	salvageIcon->setVisible(true);
	m_wSalvageButton->addChildWindow(salvageIcon);
	salvageIcon->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TowerMenu::Salvage, TowerMenu::GetSingletonPtr()));
	salvageIcon->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TowerMenu::SalvageHover, TowerMenu::GetSingletonPtr()));
	salvageIcon->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TowerMenu::SalvageLeaves, TowerMenu::GetSingletonPtr()));

	m_pMainWindow->addChildWindow(m_wSalvageButton);
	//m_wSalvageButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TowerMenu::Salvage, TowerMenu::GetSingletonPtr()));
	//m_wSalvageButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TowerMenu::SalvageHover, TowerMenu::GetSingletonPtr()));
	//m_wSalvageButton->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TowerMenu::SalvageLeaves, TowerMenu::GetSingletonPtr()));

	// Create upgrade button
	/*m_wUpgradeButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TrM_Upgrade");
	m_wUpgradeButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.4,0)));
	m_wUpgradeButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.6,0), CEGUI::UDim(0.6,0)));
	m_wUpgradeButton->setProperty( "NormalImage", "set:ButtonBackground image:BlueBackground" );
	m_wUpgradeButton->setProperty( "HoverImage", "set:ButtonBackground image:BlueBackground" );
	m_wUpgradeButton->setProperty( "PushedImage", "set:ButtonBackground image:BlueBackground" );*/

	m_wUpgradeButton = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TrM_Upgrade");
	m_wUpgradeButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.4,0)));
	m_wUpgradeButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.6,0), CEGUI::UDim(0.6,0)));
	m_wUpgradeButton->setProperty( "Image", "set:ButtonBackground image:BlueBackground" );

	CEGUI::Window* upgradeIcon = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TrM_UpgradeIcon");
	upgradeIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0.6,0), CEGUI::UDim(0.7,0)));
	upgradeIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.15,0)));
	upgradeIcon->setProperty( "NormalImage", "set:UpgradeN image:full_image" );
	upgradeIcon->setProperty( "HoverImage", "set:UpgradeH image:full_image" );
	upgradeIcon->setProperty( "PushedImage", "set:UpgradeH image:full_image" );
	upgradeIcon->setVisible(true);
	m_wUpgradeButton->addChildWindow(upgradeIcon);
	upgradeIcon->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TowerMenu::Upgrade, TowerMenu::GetSingletonPtr()));
	upgradeIcon->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TowerMenu::UpgradeHover, TowerMenu::GetSingletonPtr()));
	upgradeIcon->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TowerMenu::UpgradeLeaves, TowerMenu::GetSingletonPtr()));
	
	/*CEGUI::Window* stopText = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticText", "TrM_UT");
	stopText->setSize(CEGUI::UVector2(CEGUI::UDim(1.0,0), CEGUI::UDim(0.5,0)));
	stopText->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0.25,0)));
	stopText->setText("Not enough\nresource\nto upgrade");
	stopText->setFont("DejaVuSans-5");
	stopText->setProperty("TextColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000");
	stopText->setProperty("HorzFormatting", "HorzCentred");
	stopText->setProperty("VertFormatting", "VertCentred");*/

	CEGUI::Window* stopIcon = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "TrM_Stop");
	stopIcon->setSize(CEGUI::UVector2(CEGUI::UDim(1.0,0), CEGUI::UDim(1.0,0)));
	stopIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	stopIcon->setProperty("Image", "set:StopIcon image:full_image");
	stopIcon->setVisible(false);

	//stopIcon->addChildWindow(stopText);
	m_wUpgradeButton->addChildWindow(stopIcon);

	m_pMainWindow->addChildWindow(m_wUpgradeButton);
	//m_wUpgradeButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TowerMenu::Upgrade, TowerMenu::GetSingletonPtr()));
	//m_wUpgradeButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TowerMenu::UpgradeHover, TowerMenu::GetSingletonPtr()));
	//m_wUpgradeButton->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TowerMenu::UpgradeLeaves, TowerMenu::GetSingletonPtr()));

	// Create cancel button
	m_wCancelButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ImageButton", "TrM_Cancel");
	m_wCancelButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0.2,0)));
	m_wCancelButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.5,0)));
	m_wCancelButton->setProperty( "NormalImage", "set:Cancel image:full_image" );
	m_wCancelButton->setProperty( "HoverImage", "set:Cancel image:full_image" );
	m_wCancelButton->setProperty( "PushedImage", "set:Cancel image:full_image" );
	m_wCancelButton->setAlpha(0.6);

	m_pMainWindow->addChildWindow(m_wCancelButton);
	m_wCancelButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TowerMenu::Cancel, TowerMenu::GetSingletonPtr()));
	m_wCancelButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersArea, CEGUI::Event::Subscriber(&TowerMenu::CancelHover, TowerMenu::GetSingletonPtr()));
	m_wCancelButton->subscribeEvent(CEGUI::PushButton::EventMouseLeavesArea, CEGUI::Event::Subscriber(&TowerMenu::CancelLeaves, TowerMenu::GetSingletonPtr()));


	CEGUI::System::getSingleton().getGUISheet()->addChildWindow(m_pMainWindow);

	// Create menu show up animation
	m_pTowerMenuShowAnim = CEGUI::AnimationManager::getSingleton().createAnimation("TrM_Show");
	m_pTowerMenuShowAnim->setDuration(0.3f);
	m_pTowerMenuShowAnim->setReplayMode(CEGUI::Animation::RM_Once);

	CEGUI::Affector* affector = m_pTowerMenuShowAnim->createAffector("UnifiedSize", "UVector2");
	affector->createKeyFrame(0.0f, "{{0.0, 0.0},{0.0,0.0}}");
	affector->createKeyFrame(0.3f, "{{0.2, 0.0},{0.3,0.0}}",CEGUI::KeyFrame::Progression::P_QuadraticDecelerating);

	affector = m_pTowerMenuShowAnim->createAffector("UnifiedPosition", "UVector2");
	affector->createKeyFrame(0.0f);
	affector->createKeyFrame(0.3f);
	affector->getKeyFrameAtPosition(0.3f)->setProgression(CEGUI::KeyFrame::Progression::P_QuadraticDecelerating);

	m_pTowerMenuShow = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pTowerMenuShowAnim);
	m_pTowerMenuShow->setTargetWindow(m_pMainWindow);

	// Create menu moving animation
	m_pTowerMenuMoveAnim = CEGUI::AnimationManager::getSingleton().createAnimation("TrM_Move");
	m_pTowerMenuMoveAnim->setDuration(0.3f);
	m_pTowerMenuMoveAnim->setReplayMode(CEGUI::Animation::RM_Once);

	affector = m_pTowerMenuMoveAnim->createAffector("UnifiedPosition", "UVector2");
	affector->createKeyFrame(0.0f);
	affector->createKeyFrame(0.3f);
	affector->getKeyFrameAtPosition(0.3f)->setProgression(CEGUI::KeyFrame::Progression::P_QuadraticDecelerating);

	m_pTowerMenuMove = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pTowerMenuMoveAnim);
	m_pTowerMenuMove->setTargetWindow(m_pMainWindow);

	// Hide animation, use the same pause menu fade out animation
	m_pTowerMenuHideAnim = CEGUI::AnimationManager::getSingleton().getAnimation("PM_FadeOut");

	m_pTowerMenuHide = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pTowerMenuHideAnim);
	m_pTowerMenuHide->setTargetWindow(m_pMainWindow);

	m_bInitialized = true;

	BaseWindow::PushToBack();
}

void TowerMenu::MoveToTop()
{
	BaseWindow::MoveToTop();

	if(m_pTowerMenuHide->isRunning())
		m_pTowerMenuHide->stop();

	if(m_pMainWindow->getAlpha() != 1.0)
		m_pMainWindow->setAlpha(1.0f);

	m_pTowerMenuShowAnim->getAffectorAtIdx(1)->getKeyFrameAtPosition(0.0f)->setValue(TranslatePositionToString(m_fpWindowPos.first + 0.1f, m_fpWindowPos.second + 0.15f).c_str());
	m_pTowerMenuShowAnim->getAffectorAtIdx(1)->getKeyFrameAtPosition(0.3f)->setValue(TranslatePositionToString(m_fpWindowPos.first, m_fpWindowPos.second).c_str());

	m_pTowerMenuShow->start();
}

void TowerMenu::PushToBack()
{
	if(!m_bInitialized || !m_bIsShown)
		return;

	m_pTowerMenuHide->start();

	m_bIsShown = false;
}

void TowerMenu::MoveMenuTo(const OIS::MouseEvent& pArg)
{
	if(!m_bInitialized)
		return;

	if(!m_pTower)
		PushToBack();

	else
	{
		CalculatePosition(pArg);
		EnableUpgradeButton();

		if(!m_bIsShown)
		{
			m_pMainWindow->setPosition(CEGUI::UVector2(
				CEGUI::UDim(m_fpWindowPos.first,0.0f),
				CEGUI::UDim(m_fpWindowPos.second,0.0f)));
			
			MoveToTop();
		}

		else
		{
			//if(m_pTowerMenuMoveAnim)
			//{
			//	CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(m_pTowerMenuMoveAnim);
			//	CEGUI::AnimationManager::getSingleton().destroyAnimation(m_pTowerMenuMoveAnim);
			//}
			//
			//m_pTowerMenuMoveAnim = CEGUI::AnimationManager::getSingleton().createAnimation("TrM_Move");
			//m_pTowerMenuMoveAnim->setDuration(0.3f);
			//m_pTowerMenuMoveAnim->setReplayMode(CEGUI::Animation::RM_Once);
			//
			//CEGUI::Affector* affector = m_pTowerMenuMoveAnim->createAffector("UnifiedPosition", "UVector2");
			//affector->createKeyFrame(0.0f, TranslatePositionToString(m_pMainWindow->getPosition().d_x.d_scale, m_pMainWindow->getPosition().d_y.d_scale).c_str());
			//affector->createKeyFrame(0.3f, TranslatePositionToString(m_fpWindowPos.first, m_fpWindowPos.second).c_str(), CEGUI::KeyFrame::Progression::P_QuadraticDecelerating);
			//
			//m_pTowerMenuMove = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pTowerMenuMoveAnim);
			//m_pTowerMenuMove->setTargetWindow(m_pMainWindow);
			//m_pTowerMenuMove->start();

			/*if(m_pTowerMenuMoveAnim->getNumAffectors() != 0)
			{
				for(int i = 0; i < m_pTowerMenuMoveAnim->getNumAffectors(); ++i)
				{
					m_pTowerMenuMoveAnim->destroyAffector(m_pTowerMenuMoveAnim->getAffectorAtIdx(i));
				}
			}*/

			/*CEGUI::Affector* affector = m_pTowerMenuMoveAnim->createAffector("UnifiedPosition", "UVector2");
			
			affector->createKeyFrame(0.0f, 
				TranslatePositionToString(m_pMainWindow->getPosition().d_x.d_scale, m_pMainWindow->getPosition().d_y.d_scale));
			
			affector->createKeyFrame(0.3f,
				TranslatePositionToString(m_fpWindowPos.first, m_fpWindowPos.second),
				CEGUI::KeyFrame::Progression::P_QuadraticDecelerating);

			m_pTowerMenuMove = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pTowerMenuMoveAnim);
			m_pTowerMenuMove->setTargetWindow(m_pMainWindow);
			m_pTowerMenuMove->start();*/

			m_pTowerMenuMoveAnim->getAffectorAtIdx(0)->getKeyFrameAtPosition(0.0f)->setValue(TranslatePositionToString(m_pMainWindow->getPosition().d_x.d_scale, m_pMainWindow->getPosition().d_y.d_scale).c_str());
			//m_pTowerMenuMoveAnim->getAffectorAtIdx(0)->getKeyFrameAtPosition(0.0f)->setValue("{{0.0, 0.0},{0.0,0.0}}");
			m_pTowerMenuMoveAnim->getAffectorAtIdx(0)->getKeyFrameAtPosition(0.3f)->setValue(TranslatePositionToString(m_fpWindowPos.first, m_fpWindowPos.second).c_str());
			m_pTowerMenuMove->start();
		}
	}
}

void TowerMenu::EnableUpgradeButton()
{
	/*if (!static_cast<BaseTower*>(m_pTower)->CanInteract())
		return;*/

	if(!m_pTower)
		return;

	(MainCurrency::CanAfford(static_cast<BaseTower*>(m_pTower)->GetUpgradeCost(static_cast<BaseTower*>(m_pTower)->GetLevel())))?
		CEGUI::WindowManager::getSingleton().getWindow("TrM_Stop")->setVisible(false):
		CEGUI::WindowManager::getSingleton().getWindow("TrM_Stop")->setVisible(true);

	if(!CEGUI::WindowManager::getSingleton().getWindow("TrM_Stop")->isVisible())
		if(static_cast<BaseTower*>(m_pTower)->GetLevel() == 3)
			CEGUI::WindowManager::getSingleton().getWindow("TrM_Stop")->setVisible(true);
}

//void TowerMenu::DisableUpgradeButton()
//{
//	if(!m_bUpgradeButton)
//		return;
//
//	m_wUpgradeButton->disable();
//
//	m_bUpgradeButton = false;
//}

void TowerMenu::CheckMouseCursor()
{
	if(!m_bMouseCursorChanged)
		return;

	CEGUI::System::getSingleton().setDefaultMouseCursor("WindowsLook", "MouseArrow");
	CEGUI::MouseCursor::getSingleton().setExplicitRenderSize(CEGUI::Size(15.0,20.0));

	m_bMouseCursorChanged = false;
}

bool TowerMenu::TargetHover(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonHover.wav");
	SelectableManager::SetPause();

	return true;
}

bool TowerMenu::SalvageHover(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonHover.wav");
	SelectableManager::SetPause();

	return true;
}

bool TowerMenu::UpgradeHover(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonHover.wav");
	SelectableManager::SetPause();

	return true;
}

bool TowerMenu::CancelHover(const CEGUI::EventArgs &p_eEventArgs)
{
    SoundManager::Play2DSound("menuButtonHover.wav");
	m_wCancelButton->setAlpha(1.0);

	SelectableManager::SetPause();

	return true;
}

bool TowerMenu::TargetLeaves(const CEGUI::EventArgs &p_eEventArgs)
{
	SelectableManager::SetPause(false);

	return true;
}

bool TowerMenu::SalvageLeaves(const CEGUI::EventArgs &p_eEventArgs)
{
	SelectableManager::SetPause(false);

	return true;
}

bool TowerMenu::UpgradeLeaves(const CEGUI::EventArgs &p_eEventArgs)
{
	SelectableManager::SetPause(false);

	return true;
}

bool TowerMenu::CancelLeaves(const CEGUI::EventArgs &p_eEventArgs)
{
	m_wCancelButton->setAlpha(0.6);

	SelectableManager::SetPause(false);

	return true;
}

bool TowerMenu::Target(const CEGUI::EventArgs &p_eEventArgs)
{
	CEGUI::System::getSingleton().setDefaultMouseCursor("TargetingMouseCursor", "full_image");
	//CEGUI::MouseCursor::getSingleton().setImage("TargetingMouseCursor", "full_image");
	CEGUI::MouseCursor::getSingleton().setExplicitRenderSize(CEGUI::Size(30.0,35.0));

	m_bMouseCursorChanged = true;

	//CEGUI::System::getSingleton().setDefaultMouseCursor("WindowsLook", "MouseArrow");
	//CEGUI::MouseCursor::getSingleton().setExplicitRenderSize(CEGUI::Size(20.0,20.0)); 

	PushToBack();

	return true;
}

bool TowerMenu::Salvage(const CEGUI::EventArgs &p_eEventArgs)
{
    if(static_cast<BaseTower*>(m_pTower)->CanInteract())
    {
        if(static_cast<BaseTower*>(m_pTower)->GetTile() != 0)
	    {
		    static_cast<BaseTower*>(m_pTower)->Salvage();

		    m_pTower = 0;

		    PushToBack();
	    }
    }

	return true;
}

bool TowerMenu::Upgrade(const CEGUI::EventArgs &p_eEventArgs)
{
    if (static_cast<BaseTower*>(m_pTower)->CanInteract())
    {
        if (MainCurrency::CanAfford(static_cast<BaseTower*>(m_pTower)->GetUpgradeCost(static_cast<BaseTower*>(m_pTower)->GetLevel())))
        {
	        static_cast<Player*>(m_pPlayer)->SetTowerToUpgrade(static_cast<BaseTower*>(m_pTower));

	        if(static_cast<BaseTower*>(m_pTower)->GetTile() != 0)
	        {
		        static_cast<Player*>(m_pPlayer)->MoveTo(static_cast<BaseTower*>(m_pTower)->GetTile());
                static_cast<Player*>(m_pPlayer)->SetImpendingAction(PA_UPGRADE);

		        m_pTower = 0;

		        PushToBack();
	        }
        }
    }

	return true;
}

bool TowerMenu::Cancel(const CEGUI::EventArgs &p_eEventArgs)
{
	m_pTower = 0;

	PushToBack();

	return true;
}