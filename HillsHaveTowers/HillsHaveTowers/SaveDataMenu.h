//-------------------------------------------------------------------------------------------
//	SaveDataMenu.h											Original Author: Pierre Chen
//
//	
//-------------------------------------------------------------------------------------------

#ifndef _SAVEDATAMENU_
#define _SAVEDATAMENU_

#include "BaseWindow.h"

class SaveDataMenu: public BaseWindow
{
private:
	static SaveDataMenu* s_pSaveDataMenu;
	static unsigned int s_uiListItemCount;

	CEGUI::Listbox* m_pSaveList;
	CEGUI::Window* m_pSaveButton;
	CEGUI::Window* m_pCancelButton;

	CEGUI::AnimationInstance* m_pMenuShow;
	CEGUI::AnimationInstance* m_pMenuFadeOut;

	SaveDataMenu();
	SaveDataMenu(const SaveDataMenu& pSaveDataMenu);
	~SaveDataMenu();
	
	void operator=(const SaveDataMenu& pSaveDataMenu);

public:
	static SaveDataMenu* GetSingletonPtr();
	static void Destroy();

	virtual void Init();
	virtual void MoveToTop();
	virtual void PushToBack();

	void AddNewListItem(const std::string& p_strItemText);
	void ChangeListItem(const std::string& p_strItemText, const int p_iItemIndex);

	bool DataSelectionChanged(const CEGUI::EventArgs &p_eEventArgs);
	bool Save(const CEGUI::EventArgs &p_eEventArgs);
	bool Cancel(const CEGUI::EventArgs &p_eEventArgs);
};

#endif