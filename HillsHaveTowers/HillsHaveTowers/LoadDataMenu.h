//-------------------------------------------------------------------------------------------
//	LoadDataMenu.h											Original Author: Pierre Chen
//
//	
//-------------------------------------------------------------------------------------------

#ifndef _LOADDATAMENU_
#define _LOADDATAMENU_

#include "BaseWindow.h"

class LoadDataMenu: public BaseWindow
{
private:
	static LoadDataMenu* s_pLoadDataMenu;
	static unsigned int s_uiListItemCount;

	CEGUI::Listbox* m_pLoadList;
	CEGUI::Window* m_pLoadButton;
	CEGUI::Window* m_pCancelButton;

	CEGUI::Animation* m_pMenuAnim;

	CEGUI::AnimationInstance* m_pMenuShow;
	CEGUI::AnimationInstance* m_pMenuFadeOut;

	LoadDataMenu();
	LoadDataMenu(const LoadDataMenu& pLoadDataMenu);
	~LoadDataMenu();
	
	void operator=(const LoadDataMenu& pLoadDataMenu);

public:
	static LoadDataMenu* GetSingletonPtr();
	static void Destroy();

	virtual void Init();
	virtual void MoveToTop();
	virtual void PushToBack();

	void AddNewListItem(const std::string& p_strItemText);
	void ChangeListItem(const std::string& p_strItemText, const int p_iItemIndex);

	bool DataSelectionChanged(const CEGUI::EventArgs &p_eEventArgs);
	bool Load(const CEGUI::EventArgs &p_eEventArgs);
	bool Cancel(const CEGUI::EventArgs &p_eEventArgs);
};

#endif