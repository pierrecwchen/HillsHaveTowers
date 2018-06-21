//-------------------------------------------------------------------------------------------
//	TileMenu.h											Original Author: Pierre Chen
//
//	
//-------------------------------------------------------------------------------------------

#ifndef _TILEMENU_
#define _TILEMENU_

#include "BaseWindow.h"
#include "MainCurrency.h"
#include <OISEvents.h>
#include <OISMouse.h>

//enum TMButton
//{
//	TM_ArrowTower = 0,
//	TM_CannonTower,
//	TM_SniperTower,
//	TM_FireTower,
//	TM_SlowTower,
//	TM_NUMTMBUTTON
//};

struct TileMenuButton
{
	bool m_bArrow, m_bCannon, m_bFire, m_bSniper, m_bSlow;

	TileMenuButton():m_bArrow(true), m_bCannon(true), m_bFire(true), m_bSniper(true), m_bSlow(true)
	{
	}
	
	TileMenuButton(bool p_bArrow, bool p_bCannon, bool p_bFire, bool p_bSniper, bool p_bSlow):
		m_bArrow(p_bArrow),
		m_bCannon(p_bCannon),
		m_bFire(p_bFire),
		m_bSniper(p_bSniper),
		m_bSlow(p_bSlow)
	{
	}
	
	~TileMenuButton()
	{
	}
};

class TileMenu: public BaseWindow
{
private:
	static TileMenu* m_uiTileMenu;

	TileMenuButton m_tmButtons;

	void* m_pPlayer;
	void* m_pTile;

	std::pair<float, float> m_fpWindowPos;

	CEGUI::Window* m_wArrowTowerButton;
	CEGUI::Window* m_wCannonTowerButton;
	CEGUI::Window* m_wSniperTowerButton;
	CEGUI::Window* m_wSlowTowerButton;
	CEGUI::Window* m_wFireTowerButton;
	CEGUI::Window* m_wCancelButton;

	CEGUI::Animation* m_pTileMenuShowAnim;
	CEGUI::Animation* m_pTileMenuMoveAnim;
	CEGUI::Animation* m_pTileMenuHideAnim;

	CEGUI::AnimationInstance* m_pTileMenuShow;
	CEGUI::AnimationInstance* m_pTileMenuMove;
	CEGUI::AnimationInstance* m_pTileMenuHide;

	TileMenu();
	TileMenu(const TileMenu& p_uiTileMenu);
	~TileMenu();

	void operator=(const TileMenu& p_uiTileMenu);

	void CalculatePosition(const OIS::MouseEvent& pArg);
	std::string TranslatePositionToString(const float p_fXPos, const float p_fYPos);

public:
	static TileMenu* GetSingletonPtr();
	static void Destroy();
	
	virtual void Init();

	virtual void MoveToTop();
	virtual void PushToBack();
	void MoveMenuTo(const OIS::MouseEvent& pArg);

	void EnableButton();

	inline void SetPlayer(void* p_pPlayer){ m_pPlayer = p_pPlayer; }
	inline void SetTileToMove(void* p_pTile){ m_pTile = p_pTile; }

	bool ArrowTowerHover(const CEGUI::EventArgs &p_eEventArgs);
	bool CannonTowerHover(const CEGUI::EventArgs &p_eEventArgs);
	bool SniperTowerHover(const CEGUI::EventArgs &p_eEventArgs);
	bool FireTowerHover(const CEGUI::EventArgs &p_eEventArgs);
	bool SlowTowerHover(const CEGUI::EventArgs &p_eEventArgs);
	bool CancelHover(const CEGUI::EventArgs &p_eEventArgs);

	bool ArrowTowerLeaves(const CEGUI::EventArgs &p_eEventArgs);
	bool CannonTowerLeaves(const CEGUI::EventArgs &p_eEventArgs);
	bool SniperTowerLeaves(const CEGUI::EventArgs &p_eEventArgs);
	bool FireTowerLeaves(const CEGUI::EventArgs &p_eEventArgs);
	bool SlowTowerLeaves(const CEGUI::EventArgs &p_eEventArgs);
	bool CancelLeaves(const CEGUI::EventArgs &p_eEventArgs);

	bool ArrowTower(const CEGUI::EventArgs &p_eEventArgs);
	bool CannonTower(const CEGUI::EventArgs &p_eEventArgs);
	bool SniperTower(const CEGUI::EventArgs &p_eEventArgs);
	bool FireTower(const CEGUI::EventArgs &p_eEventArgs);
	bool SlowTower(const CEGUI::EventArgs &p_eEventArgs);
	bool Cancel(const CEGUI::EventArgs &p_eEventArgs);
};

#endif