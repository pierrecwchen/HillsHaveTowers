//-------------------------------------------------------------------------------------------
//	TowerMenu.h											Original Author: Pierre Chen
//
//	
//-------------------------------------------------------------------------------------------

#ifndef _TOWERMENU_
#define _TOWERMENU_

#include "BaseWindow.h"
#include <OISEvents.h>
#include <OISMouse.h>

class TowerMenu: public BaseWindow
{
private:
	static TowerMenu* m_uiTowerMenu;

	bool m_bMouseCursorChanged;
	bool m_bUpgradeButton;

	void* m_pPlayer;
	void* m_pTower;

	std::pair<float, float> m_fpWindowPos;

	CEGUI::Window* m_wTargetButton;
	CEGUI::Window* m_wSalvageButton;
	CEGUI::Window* m_wUpgradeButton;
	CEGUI::Window* m_wCancelButton;
	
	CEGUI::Animation* m_pTowerMenuShowAnim;
	CEGUI::Animation* m_pTowerMenuMoveAnim;
	CEGUI::Animation* m_pTowerMenuHideAnim;

	CEGUI::AnimationInstance* m_pTowerMenuShow;
	CEGUI::AnimationInstance* m_pTowerMenuMove;
	CEGUI::AnimationInstance* m_pTowerMenuHide;

	TowerMenu();
	TowerMenu(const TowerMenu& p_uiTowerMenu);
	~TowerMenu();

	void operator=(const TowerMenu& p_uiTowerMenu);

	void CalculatePosition(const OIS::MouseEvent& pArg);
	std::string TranslatePositionToString(const float p_fXPos, const float p_fYPos);

public:
	static TowerMenu* GetSingletonPtr();
	static void Destroy();
	
	virtual void Init();

	virtual void MoveToTop();
	virtual void PushToBack();
	void MoveMenuTo(const OIS::MouseEvent& pArg);

	void EnableUpgradeButton();
	//void DisableUpgradeButton();

	inline void SetPlayer(void* p_pPlayer){ m_pPlayer = p_pPlayer; }
	inline void SetTower(void* p_pTower){ m_pTower = p_pTower; }
	void CheckMouseCursor();

	bool TargetHover(const CEGUI::EventArgs &p_eEventArgs);
	bool SalvageHover(const CEGUI::EventArgs &p_eEventArgs);
	bool UpgradeHover(const CEGUI::EventArgs &p_eEventArgs);
	bool CancelHover(const CEGUI::EventArgs &p_eEventArgs);

	bool TargetLeaves(const CEGUI::EventArgs &p_eEventArgs);
	bool SalvageLeaves(const CEGUI::EventArgs &p_eEventArgs);
	bool UpgradeLeaves(const CEGUI::EventArgs &p_eEventArgs);
	bool CancelLeaves(const CEGUI::EventArgs &p_eEventArgs);

	bool Target(const CEGUI::EventArgs &p_eEventArgs);
	bool Salvage(const CEGUI::EventArgs &p_eEventArgs);
	bool Upgrade(const CEGUI::EventArgs &p_eEventArgs);
	bool Cancel(const CEGUI::EventArgs &p_eEventArgs);
};

#endif