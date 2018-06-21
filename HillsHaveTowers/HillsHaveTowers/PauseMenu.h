//-------------------------------------------------------------------------------------------
//	PauseMenu.h											Original Author: Pierre Chen
//
//	A singleton class which give the functionalities for pause menu
//-------------------------------------------------------------------------------------------

#ifndef _PAUSEMENU_
#define _PAUSEMENU_

#include "BaseWindow.h"

class PauseMenu: public BaseWindow
{
private:
	static PauseMenu* m_pmPauseMenu;

	bool m_bQuit;

	CEGUI::Window* m_pPauseMenu;

	CEGUI::Animation* m_pBombAnim;
	CEGUI::Animation* m_pPauseMenuAnim;
	CEGUI::Animation* m_pPauseMenuFadeOutAnim;

	CEGUI::AnimationInstance* m_pBombFlyIn;
	CEGUI::AnimationInstance* m_pPauseMenuResize;
	CEGUI::AnimationInstance* m_pPauseMenuFadeOut;

	PauseMenu();
	PauseMenu(const PauseMenu& p_pmPauseMenu);
	~PauseMenu();
	
	void operator=(const PauseMenu& p_pmPauseMenu);

public:
	static PauseMenu* GetSingletonPtr();
	static void Destroy();

	virtual void Init();
	virtual void MoveToTop();
	virtual void PushToBack();

	void ResumeGame();

	bool ResumeGame(const CEGUI::EventArgs &p_eEventArgs);
	bool LoadLastCheckPoint(const CEGUI::EventArgs &p_eEventArgs);
	bool RestartLevel(const CEGUI::EventArgs &p_eEventArgs);
	bool SaveGame(const CEGUI::EventArgs &p_eEventArgs);
	bool LoadGame(const CEGUI::EventArgs &p_eEventArgs);
	bool Settings(const CEGUI::EventArgs &p_eEventArgs);
	bool ExitToMainMenu(const CEGUI::EventArgs &p_eEventArgs);
	bool ExitGame(const CEGUI::EventArgs &p_eEventArgs);

	inline bool IsQuit(){ return m_bQuit; }
};

#endif