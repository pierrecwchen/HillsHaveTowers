#ifndef _OPTIONMENU_
#define _OPTIONMENU_

#include "BaseWindow.h"

class OptionMenu: public BaseWindow
{
private:
	static OptionMenu* m_uiOptionMenu;

protected:
	OptionMenu();
	OptionMenu(const OptionMenu& pOptionMenu);
	virtual ~OptionMenu();

	void operator=(const OptionMenu& pOptionMenu);


public:
	static OptionMenu* GetSingletonPtr();
	static void Destroy();

	virtual void Init();
	virtual void MoveToTop();
	virtual void PushToBack();
};

#endif