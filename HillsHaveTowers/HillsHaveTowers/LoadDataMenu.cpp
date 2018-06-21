#include "LoadDataMenu.h"
#include "UIManager.h"
#include "states\StateManager.h"
#include "states\FrontEndState.h"
#include "states\GameState.h"

LoadDataMenu* LoadDataMenu::s_pLoadDataMenu = 0;
unsigned int LoadDataMenu::s_uiListItemCount = 1;

LoadDataMenu::LoadDataMenu(): m_pLoadList(0)
{
}

LoadDataMenu::LoadDataMenu(const LoadDataMenu& pLoadDataMenu)
{
}

LoadDataMenu::~LoadDataMenu()
{
	CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(m_pMenuAnim);
	CEGUI::AnimationManager::getSingleton().destroyAnimation(m_pMenuAnim);
}

void LoadDataMenu::operator=(const LoadDataMenu& pLoadDataMenu)
{
}

LoadDataMenu* LoadDataMenu::GetSingletonPtr()
{
	if(!s_pLoadDataMenu)
		s_pLoadDataMenu = new LoadDataMenu();

	return s_pLoadDataMenu;
}

void LoadDataMenu::Destroy()
{
	delete s_pLoadDataMenu;
	s_pLoadDataMenu = 0;
}

void LoadDataMenu::Init()
{
	if(m_bInitialized)
		return;

	// Create base menu
	m_pMainWindow = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "LoadDataMenu");
	m_pMainWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.5,0)));
	m_pMainWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35,0), CEGUI::UDim(0.25,0)));
	m_pMainWindow->setProperty("Image", "set:LoadDataMenuBack image:full_image");

	m_pLoadList = static_cast<CEGUI::Listbox*>(CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Listbox", "LDM_LoadList"));
	m_pLoadList->setSize(CEGUI::UVector2(CEGUI::UDim(0.8,0), CEGUI::UDim(0.7,0)));
	m_pLoadList->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1,0), CEGUI::UDim(0.1,0)));
	m_pLoadList->setMultiselectEnabled(false);
	m_pLoadList->subscribeEvent(CEGUI::Listbox::EventSelectionChanged, CEGUI::Event::Subscriber(&LoadDataMenu::DataSelectionChanged, LoadDataMenu::GetSingletonPtr()));

	/*CEGUI::ListboxTextItem* itemListbox = new CEGUI::ListboxTextItem("Value A", 1);
		itemListbox->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
		itemListbox->setAutoDeleted(true);
		m_pLoadList->addItem(itemListbox);
	itemListbox = new CEGUI::ListboxTextItem("Value B", 2);
		itemListbox->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
		itemListbox->setAutoDeleted(true);
		m_pLoadList->addItem(itemListbox);
	itemListbox = new CEGUI::ListboxTextItem("Value C", 3);
		itemListbox->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
		itemListbox->setAutoDeleted(true);
		m_pLoadList->addItem(itemListbox);
	itemListbox = new CEGUI::ListboxTextItem("Value D", 4);
		itemListbox->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
		itemListbox->setAutoDeleted(true);
		m_pLoadList->addItem(itemListbox);
	m_pLoadList->ensureItemIsVisible(itemListbox);*/

	m_pMainWindow->addChildWindow(m_pLoadList);

	m_pLoadButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "LDM_Load");
	m_pLoadButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.1,0)));
	m_pLoadButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15,0), CEGUI::UDim(0.9,0)));
	m_pLoadButton->setText("Load");
	m_pLoadButton->disable();
	m_pLoadButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LoadDataMenu::Load, LoadDataMenu::GetSingletonPtr()));

	m_pMainWindow->addChildWindow(m_pLoadButton);

	m_pCancelButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "LDM_Cancel");
	m_pCancelButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.1,0)));
	m_pCancelButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.55,0), CEGUI::UDim(0.9,0)));
	m_pCancelButton->setText("Cancel");
	m_pCancelButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LoadDataMenu::Cancel, LoadDataMenu::GetSingletonPtr()));

	m_pMainWindow->addChildWindow(m_pCancelButton);

	// Add the pause menu to the root
	CEGUI::System::getSingleton().getGUISheet()->addChildWindow(m_pMainWindow);

	// Create pause menu animation
	// Show up animation
	m_pMenuAnim = CEGUI::AnimationManager::getSingleton().createAnimation("LDM_Show");
	m_pMenuAnim->setDuration(0.5f);
	m_pMenuAnim->setReplayMode(CEGUI::Animation::RM_Once);

	CEGUI::Affector* affector = m_pMenuAnim->createAffector("UnifiedAreaRect", "URect");
	affector->createKeyFrame(0.0f, "{{0.5,0.0}, {0.5,0.0}, {0.5,0.0}, {0.5,0.0}}");
	affector->createKeyFrame(0.25f, "{{0.5,0.0}, {0.5,0.0}, {0.5,0.0}, {0.5,0.0}}");
	affector->createKeyFrame(0.5f, "{{0.35,0.0}, {0.25,0.0}, {0.65,0.0}, {0.75,0.0}}", CEGUI::KeyFrame::Progression::P_QuadraticAccelerating);

	m_pMenuShow = CEGUI::AnimationManager::getSingleton().instantiateAnimation(m_pMenuAnim);
	m_pMenuShow->setTargetWindow(m_pMainWindow);

	m_pMenuFadeOut = CEGUI::AnimationManager::getSingleton().instantiateAnimation("PM_FadeOut");
	m_pMenuFadeOut->setTargetWindow(m_pMainWindow);

	m_bInitialized = true;

	BaseWindow::PushToBack();
}

void LoadDataMenu::MoveToTop()
{
	BaseWindow::MoveToTop();

	m_pLoadList->clearAllSelections();

	if(m_pMenuFadeOut->isRunning())
		m_pMenuFadeOut->stop();

	if(m_pMainWindow->getAlpha() != 1.0)
		m_pMainWindow->setAlpha(1.0);

	m_pMenuShow->start();
}

void LoadDataMenu::PushToBack()
{
	if(!m_bInitialized || !m_bIsShown)
		return;

	m_pMenuFadeOut->start();

	m_bIsShown = false;
}

void LoadDataMenu::AddNewListItem(const std::string& p_strItemText)
{
	s_uiListItemCount++;
	CEGUI::ListboxTextItem* itemListbox = new CEGUI::ListboxTextItem(p_strItemText, s_uiListItemCount);
	itemListbox->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
	itemListbox->setAutoDeleted(true);
	m_pLoadList->addItem(itemListbox);
	m_pLoadList->ensureItemIsVisible(itemListbox);
}

void LoadDataMenu::ChangeListItem(const std::string& p_strItemText, const int p_iItemIndex)
{
	CEGUI::ListboxTextItem* itemListbox = dynamic_cast<CEGUI::ListboxTextItem*>(m_pLoadList->getListboxItemFromIndex(p_iItemIndex));
	itemListbox->setText(p_strItemText);

	m_pLoadList->handleUpdatedItemData();
}

bool LoadDataMenu::DataSelectionChanged(const CEGUI::EventArgs &p_eEventArgs)
{
	if(m_pLoadButton->isDisabled())
		m_pLoadButton->enable();
	return true;
}

bool LoadDataMenu::Load(const CEGUI::EventArgs &p_eEventArgs)
{

	return true;
}

bool LoadDataMenu::Cancel(const CEGUI::EventArgs &p_eEventArgs)
{
	if(dynamic_cast<Hills::FrontEndState*>(Hills::StateManager::GetCurrentState()))
	{
		UIManager::SwitchWindows("MainMenu", "LoadDataMenu");
		MainMenu::GetSingletonPtr()->GetMainWindowPtr()->enable();
	}
	else if(dynamic_cast<Hills::GameState*>(Hills::StateManager::GetCurrentState()))
		UIManager::SwitchWindows("PauseMenu");
	else
	{
		printf("Load data menu error: Undefined current state");
		return false;
	}
	return true;
}