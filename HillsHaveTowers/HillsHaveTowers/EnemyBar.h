//-------------------------------------------------------------------------------------------
//	EnemyBar.h											Original Author: Pierre Chen
//
//	
//-------------------------------------------------------------------------------------------

#ifndef _ENEMYBAR_
#define _ENEMYBAR_

#include "BaseWindow.h"
#include <map>
#include <vector>

class EnemyBar: public BaseWindow
{
private:
	static EnemyBar* m_uiEnemyBar;

	int m_iAnimCount;
	int m_iWindowCount;
	int m_iCurrentEnemyGroup;
	float m_fSpeedFactor;

	std::map<float, CEGUI::Animation*> m_mapAnimDef;
	std::vector<std::pair<CEGUI::AnimationInstance*, CEGUI::Window*>> m_vAnimations;
	std::vector<CEGUI::AnimationInstance*> m_vPausedAnim;

	EnemyBar();
	EnemyBar(const EnemyBar& p_uiEnemyBar);
	~EnemyBar();

	void operator=(const EnemyBar& p_uiEnemyBar);

public:
	static EnemyBar* GetSingletonPtr();
	static void Destroy();

	virtual void Init();
	virtual void MoveToTop();
	virtual void PushToBack();
	void CreateAnimation(int p_iEnemyType, int p_iEnemyCount, float p_fDuration, bool p_bFast = false);
	void StartAnim(int p_iGroups);
	void StartAnimWithWave(int p_iGroup, int p_iGroupNum);
	void SendWave(int p_iGroupNum);
	void Pause();
	void Restart();
	void SpeedUp();
	void SlowDown();
	void ResetAnim();

	void Reset();
};

#endif