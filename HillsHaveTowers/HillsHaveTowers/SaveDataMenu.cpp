#include "SaveDataMenu.h"
#include "UIManager.h"
#include "GameDataManager.h"

SaveDataMenu* SaveDataMenu::s_pSaveDataMenu = 0;
unsigned int SaveDataMenu::s_uiListItemCount = 1;

SaveDataMenu::SaveDataMenu(): m_pSaveList(0)
{
}

SaveDataMenu::SaveDataMenu(const SaveDataMenu& pSaveDataMenu)
{
}

SaveDataMenu::~SaveDataMenu()
{
}

void SaveDataMenu::operator=(const SaveDataMenu& pSaveDataMenu)
{
}

SaveDataMenu* SaveDataMenu::GetSingletonPtr()
{
	if(!s_pSaveDataMenu)
		s_pSaveDataMenu = new SaveDataMenu();

	return s_pSaveDataMenu;
}

void SaveDataMenu::Destroy()
{
	delete s_pSaveDataMenu;
	s_pSaveDataMenu = 0;
}

void SaveDataMenu::Init()
{
	if(m_bInitialized)
		return;

	// Create base menu
	m_pMainWindow = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "SaveDataMenu");
	m_pMainWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.5,0)));
	m_pMainWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.35,0), CEGUI::UDim(0.25,0)));
	m_pMainWindow->setProperty("Image", "set:SaveDataMenuBack image:full_image");

	m_pSaveList = static_cast<CEGUI::Listbox*>(CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Listbox", "SDM_SaveList"));
	m_pSaveList->setSize(CEGUI::UVector2(CEGUI::UDim(0.8,0), CEGUI::UDim(0.7,0)));
	m_pSaveList->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1,0), CEGUI::UDim(0.1,0)));
	m_pSaveList->setMultiselectEnabled(false);
	m_pSaveList->subscribeEvent(CEGUI::Listbox::EventSelectionChanged, CEGUI::Event::Subscriber(&SaveDataMenu::DataSelectionChanged, SaveDataMenu::GetSingletonPtr()));

	CEGUI::ListboxTextItem* itemListbox = new CEGUI::ListboxTextItem("Empty Data", 1);
	itemListbox->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
	itemListbox->setAutoDeleted(true);
	m_pSaveList->addItem(itemListbox);
	m_pSaveList->ensureItemIsVisible(itemListbox);

	m_pMainWindow->addChildWindow(m_pSaveList);

	m_pSaveButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "SDM_Save");
	m_pSaveButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.1,0)));
	m_pSaveButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15,0), CEGUI::UDim(0.9,0)));
	m_pSaveButton->setText("Save");
	m_pSaveButton->disable();
	m_pSaveButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SaveDataMenu::Save, SaveDataMenu::GetSingletonPtr()));

	m_pMainWindow->addChildWindow(m_pSaveButton);

	m_pCancelButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "SDM_Cancel");
	m_pCancelButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0), CEGUI::UDim(0.1,0)));
	m_pCancelButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.55,0), CEGUI::UDim(0.9,0)));
	m_pCancelButton->setText("Cancel");
	m_pCancelButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SaveDataMenu::Cancel, SaveDataMenu::GetSingletonPtr()));

	m_pMainWindow->addChildWindow(m_pCancelButton);

	// Add the save data menu to the root
	CEGUI::System::getSingleton().getGUISheet()->addChildWindow(m_pMainWindow);

	// Initiate save data menu animation
	m_pMenuShow = CEGUI::AnimationManager::getSingleton().instantiateAnimation("LDM_Show");
	m_pMenuShow->setTargetWindow(m_pMainWindow);

	m_pMenuFadeOut = CEGUI::AnimationManager::getSingleton().instantiateAnimation("PM_FadeOut");
	m_pMenuFadeOut->setTargetWindow(m_pMainWindow);

	m_bInitialized = true;

	BaseWindow::PushToBack();
}

void SaveDataMenu::MoveToTop()
{
	BaseWindow::MoveToTop();

	m_pSaveList->clearAllSelections();

	if(m_pMenuFadeOut->isRunning())
		m_pMenuFadeOut->stop();

	if(m_pMainWindow->getAlpha() != 1.0)
		m_pMainWindow->setAlpha(1.0);

	m_pMenuShow->start();
}

void SaveDataMenu::PushToBack()
{
	if(!m_bInitialized || !m_bIsShown)
		return;

	m_pMenuFadeOut->start();

	m_bIsShown = false;
}

void SaveDataMenu::AddNewListItem(const std::string& p_strItemText)
{
	s_uiListItemCount++;
	CEGUI::ListboxTextItem* itemListbox = new CEGUI::ListboxTextItem(p_strItemText, s_uiListItemCount);
	itemListbox->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
	itemListbox->setAutoDeleted(true);
	m_pSaveList->addItem(itemListbox);

	LoadDataMenu::GetSingletonPtr()->AddNewListItem(p_strItemText);
}

void SaveDataMenu::ChangeListItem(const std::string& p_strItemText, const int p_iItemIndex)
{
	CEGUI::ListboxTextItem* itemListbox = dynamic_cast<CEGUI::ListboxTextItem*>(m_pSaveList->getListboxItemFromIndex(p_iItemIndex));
	itemListbox->setText(p_strItemText);

	m_pSaveList->handleUpdatedItemData();

	LoadDataMenu::GetSingletonPtr()->ChangeListItem(p_strItemText, p_iItemIndex - 1);
}

bool SaveDataMenu::DataSelectionChanged(const CEGUI::EventArgs &p_eEventArgs)
{
	if(m_pSaveButton->isDisabled())
		m_pSaveButton->enable();
	return true;
}

bool SaveDataMenu::Save(const CEGUI::EventArgs &p_eEventArgs)
{
	if(!dynamic_cast<CEGUI::ListboxTextItem*>(m_pSaveList->getFirstSelectedItem()))
	{
		printf("Cannot locate list box text item in save data menu");
		return false;
	}

	if(dynamic_cast<CEGUI::ListboxTextItem*>(m_pSaveList->getFirstSelectedItem())->getID() == 1)
	{
		GameDataManager::GetSingletonPtr()->Save();
		AddNewListItem(GameDataManager::GetSingletonPtr()->GetCurrentGameData()->GetFileName());
		//GameDataManager::GetSingletonPtr()->AddGameData();
	}

	else
	{
		if(GameDataManager::GetSingletonPtr()->ChangeToGameData(static_cast<unsigned int>(dynamic_cast<CEGUI::ListboxTextItem*>(m_pSaveList->getFirstSelectedItem())->getID() - 2)))
		{
			GameDataManager::GetSingletonPtr()->Save();
			ChangeListItem(GameDataManager::GetSingletonPtr()->GetCurrentGameData()->GetFileName(),
							static_cast<unsigned int>(dynamic_cast<CEGUI::ListboxTextItem*>(m_pSaveList->getFirstSelectedItem())->getID() - 1));
		}
	}
	
	UIManager::SwitchWindows("PauseMenu");
	
	return true;
}

bool SaveDataMenu::Cancel(const CEGUI::EventArgs &p_eEventArgs)
{
	UIManager::SwitchWindows("PauseMenu");

	return true;
}