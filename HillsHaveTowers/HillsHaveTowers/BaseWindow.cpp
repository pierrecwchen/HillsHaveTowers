//-------------------------------------------------------------------------------------------
//	BaseWindow.cpp											Original Author: Pierre Chen
//
//	Please see BaseWindow.h for notes.
//-------------------------------------------------------------------------------------------

#include "BaseWindow.h"

//-------------------------------------------------------------------------------------------
//	Protected default constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
BaseWindow::BaseWindow(): m_bInitialized(false), m_bIsShown(true)
{
}

//-------------------------------------------------------------------------------------------
//	Protected destructor. (By Pierre)
//-------------------------------------------------------------------------------------------
BaseWindow::~BaseWindow()
{
	m_pMainWindow = 0;
}

//-------------------------------------------------------------------------------------------
//	Switch two windows. Can choose to hide the window which been pushed to back. Hide the 
//	window by default. (By Pierre)
//-------------------------------------------------------------------------------------------
void BaseWindow::SwitchWindows(const std::string& p_strWindowToFront, const std::string& p_strWindowToBack, bool p_bHideBackWindow)
{
	if(CEGUI::WindowManager::getSingleton().getWindow(p_strWindowToFront) != 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow(p_strWindowToFront)->setAlwaysOnTop(true);

		if(!CEGUI::WindowManager::getSingleton().getWindow(p_strWindowToFront)->isVisible())
			CEGUI::WindowManager::getSingleton().getWindow(p_strWindowToFront)->setVisible(true);
		
		CEGUI::WindowManager::getSingleton().getWindow(p_strWindowToFront)->enable();
	}

	if(CEGUI::WindowManager::getSingleton().getWindow(p_strWindowToBack) != 0)
	{
		CEGUI::WindowManager::getSingleton().getWindow(p_strWindowToBack)->setAlwaysOnTop(false);
		
		if(p_bHideBackWindow)
			CEGUI::WindowManager::getSingleton().getWindow(p_strWindowToBack)->setVisible(false);

		CEGUI::WindowManager::getSingleton().getWindow(p_strWindowToBack)->disable();
	}
}

//-------------------------------------------------------------------------------------------
//	Default move base window to top. (By Pierre)
//-------------------------------------------------------------------------------------------
void BaseWindow::MoveToTop()
{
	if(!m_bInitialized || m_bIsShown)
		return;

	m_pMainWindow->setAlwaysOnTop(true);
	m_pMainWindow->enable();
	m_pMainWindow->setVisible(true);
	
	m_bIsShown = true;
}

//-------------------------------------------------------------------------------------------
//	Default push base window to back. (By Pierre)
//-------------------------------------------------------------------------------------------
void BaseWindow::PushToBack()
{
	if(!m_bInitialized || !m_bIsShown)
		return;

	m_pMainWindow->setAlwaysOnTop(false);
	m_pMainWindow->disable();
	m_pMainWindow->setVisible(false);
	
	m_bIsShown = false;
}