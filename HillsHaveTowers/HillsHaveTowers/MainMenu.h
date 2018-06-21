//-------------------------------------------------------------------------------------------
//	MainMenu.h											Original Author: Pierre Chen
//
//	A singleton class which give the functionalities for main menu.
//-------------------------------------------------------------------------------------------

#ifndef _MAINMENU_
#define _MAINMENU_

#include "BaseWindow.h"
#include "InGameHUD.h"

class MainMenu: public BaseWindow
{
private:
	static MainMenu* m_mmMainMenu;

	bool m_bQuit;

	CEGUI::Window* m_pStartButton;
	CEGUI::Window* m_pLoadButton;
	CEGUI::Window* m_pOptionButton;
	CEGUI::Window* m_pQuitButton;
	CEGUI::Window* m_pPrototypeButton;
	CEGUI::Window* m_pTerrainButton;

	CEGUI::Animation* m_pButtonHoverAnim;
	CEGUI::Animation* m_pButtonFlyInAnimL;
	CEGUI::Animation* m_pButtonFlyInAnimR;

	CEGUI::AnimationInstance* m_pStartButtonH;
	CEGUI::AnimationInstance* m_pStartButtonFIn;
	CEGUI::AnimationInstance* m_pLoadButtonH;
	CEGUI::AnimationInstance* m_pLoadButtonFIn;
	CEGUI::AnimationInstance* m_pOptionButtonH;
	CEGUI::AnimationInstance* m_pOptionButtonFIn;
	CEGUI::AnimationInstance* m_pQuitButtonH;
	CEGUI::AnimationInstance* m_pQuitButtonFIn;

	MainMenu();
	MainMenu(const MainMenu& p_wMainMenu);
	~MainMenu();

	void operator=(const MainMenu& p_wMainMenu);

public:
	static MainMenu* GetSingletonPtr();
	static void Destroy();

	virtual void Init();
	virtual void MoveToTop();
	virtual void PushToBack();
	bool StartGame(const CEGUI::EventArgs &p_eEventArgs);
	bool LoadGame(const CEGUI::EventArgs &p_eEventArgs);
	bool Option(const CEGUI::EventArgs &p_eEventArgs);
	bool QuitGame(const CEGUI::EventArgs &p_eEventArgs);
	bool StartGameHover(const CEGUI::EventArgs &p_eEventArgs);
	bool LoadGameHover(const CEGUI::EventArgs &p_eEventArgs);
	bool OptionHover(const CEGUI::EventArgs &p_eEventArgs);
	bool QuitGameHover(const CEGUI::EventArgs &p_eEventArgs);
	bool PrototypeState(const CEGUI::EventArgs &p_eEventArgs);
	bool TerrainTestState(const CEGUI::EventArgs &p_eEventArgs);

	inline bool IsQuit(){ return m_bQuit; }
};

#endif