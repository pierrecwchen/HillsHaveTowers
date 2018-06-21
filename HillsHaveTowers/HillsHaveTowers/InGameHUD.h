//-------------------------------------------------------------------------------------------
//	InGameHUD.h											Original Author: Pierre Chen
//
//	A singleton class which give the functionalities for in-game HUD
//-------------------------------------------------------------------------------------------

#ifndef _INGAMEHUD_
#define _INGAMEHUD_

#include "BaseWindow.h"

class InGameHUD: public BaseWindow
{
private:
	static InGameHUD* m_uiInGameHUD;

	int m_iCoinStats;
	int m_iWoodStats;
	int m_iStoneStats;
	int m_iIronStats;

	CEGUI::Window* m_pVillagerCounter;

	CEGUI::Window* m_pPauseButton;
	CEGUI::Window* m_pSlowDownButton;
	CEGUI::Window* m_pSpeedUpButton;
	CEGUI::Window* m_pStartWaveButton;

	CEGUI::Animation* m_pButtonHoverAnim;

	CEGUI::AnimationInstance* m_pPauseButtonAnim;
	CEGUI::AnimationInstance* m_pSlowDownButtonAnim;
	CEGUI::AnimationInstance* m_pSpeedUpButtonAnim;
	CEGUI::AnimationInstance* m_pStartWaveButtonAnim;

	InGameHUD();
	InGameHUD(const InGameHUD& p_uiInGameHUD);
	~InGameHUD();

	void operator=(const InGameHUD& p_uiInGameHUD);

public:
	static InGameHUD* GetSingletonPtr();
	static void Destroy();

	virtual void Init();
	virtual void MoveToTop();
	virtual void PushToBack();

	void PauseGame();

	bool StartWave(const CEGUI::EventArgs &p_eEventArgs);
	bool Pause(const CEGUI::EventArgs &p_eEventArgs);
	bool SlowDown(const CEGUI::EventArgs &p_eEventArgs);
	bool SpeedUp(const CEGUI::EventArgs &p_eEventArgs);

	bool InGameHUDHover(const CEGUI::EventArgs &p_eEventArgs);
	bool InGameHUDLeaves(const CEGUI::EventArgs &p_eEventArgs);
	bool StartWaveHover(const CEGUI::EventArgs &p_eEventArgs);
	bool PauseHover(const CEGUI::EventArgs &p_eEventArgs);
	bool SlowDownHover(const CEGUI::EventArgs &p_eEventArgs);
	bool SpeedUpHover(const CEGUI::EventArgs &p_eEventArgs);

	void SetCoinStats(int p_iAmount);
	void SetWoodStats(int p_iAmount);
	void SetStoneStats(int p_iAmount);
	void SetIronStats(int p_iAmount);

	void IncreaseCoinStats();
	void IncreaseWoodStats();
	void IncreaseStoneStats();
	void IncreaseIronStats();
};

#endif