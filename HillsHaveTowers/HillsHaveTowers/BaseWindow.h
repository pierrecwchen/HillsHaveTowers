//-------------------------------------------------------------------------------------------
//	BaseWindow.h											Original Author: Pierre Chen
//
//	A class which give the interface of window/menu, and provides essential methods
//-------------------------------------------------------------------------------------------

#ifndef _BASEWINDOW_
#define _BASEWINDOW_

#include <string>
#include "CEGUI.h"

class BaseWindow
{
protected:
	bool m_bInitialized;
	bool m_bIsShown;

	CEGUI::Window* m_pMainWindow;

	BaseWindow();
	~BaseWindow();

public:
	static void SwitchWindows(const std::string& p_strWindowToFront, const std::string& p_strWindowToBack, bool p_bHideBackWindow = true);

	virtual void Init() = 0; // Override it.
	virtual void MoveToTop();
	virtual void PushToBack();

	CEGUI::Window* GetMainWindowPtr(){ return m_pMainWindow; }

};

#endif