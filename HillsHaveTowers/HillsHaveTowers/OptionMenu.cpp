#include "OptionMenu.h"

OptionMenu* OptionMenu::m_uiOptionMenu = 0;

OptionMenu::OptionMenu()
{
}

OptionMenu::OptionMenu(const OptionMenu& pOptionMenu)
{
}

OptionMenu::~OptionMenu()
{
}

void OptionMenu::operator=(const OptionMenu& pOptionMenu)
{
}

OptionMenu* OptionMenu::GetSingletonPtr()
{
	if(!m_uiOptionMenu)
		m_uiOptionMenu = new OptionMenu();

	return m_uiOptionMenu;
}

void OptionMenu::Destroy()
{
	delete m_uiOptionMenu;
	m_uiOptionMenu = 0;
}

void OptionMenu::Init()
{
	if(m_bInitialized)
		return;

	m_pMainWindow = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "OptionMenu");
	m_pMainWindow->setSize(CEGUI::UVector2(CEGUI::UDim(1,0), CEGUI::UDim(1,0)));
	m_pMainWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));

	m_bInitialized = true;

	BaseWindow::PushToBack();
}

void OptionMenu::MoveToTop()
{
	BaseWindow::MoveToTop();
}

void OptionMenu::PushToBack()
{
	BaseWindow::PushToBack();
}