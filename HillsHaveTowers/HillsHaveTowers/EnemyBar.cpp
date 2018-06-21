//-------------------------------------------------------------------------------------------
//	EnemyBar.cpp											Original Author: Pierre Chen
//
//	Please see notes from EnemyBar.h
//-------------------------------------------------------------------------------------------

#include "EnemyBar.h"
#include "states\StateManager.h"
#include "states\PrototypeState.h"
#include "states\GameState.h"
#include <iostream>
#include "UIManager.h"

EnemyBar* EnemyBar::m_uiEnemyBar = 0;

//-------------------------------------------------------------------------------------------
//	Private default constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
EnemyBar::EnemyBar(): m_iCurrentEnemyGroup(0),
					m_iAnimCount(0),
					m_iWindowCount(0),
					m_fSpeedFactor(1.0f)
{
}

//-------------------------------------------------------------------------------------------
//	Private copy constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
EnemyBar::EnemyBar(const EnemyBar& p_uiEnemyBar)
{
}

//-------------------------------------------------------------------------------------------
//	Private destructor. (By Pierre)
//-------------------------------------------------------------------------------------------
EnemyBar::~EnemyBar()
{
	for(int i = 0; i < m_vAnimations.size(); i++)
	{
		CEGUI::WindowManager::getSingleton().destroyWindow(m_vAnimations[i].second);
		CEGUI::AnimationManager::getSingleton().destroyAnimationInstance(m_vAnimations[i].first);
	}

	std::map<float, CEGUI::Animation*>::iterator animDefIt = m_mapAnimDef.begin();
	for( ; animDefIt != m_mapAnimDef.end(); animDefIt++)
	{
		CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(animDefIt->second); // Double check
		CEGUI::AnimationManager::getSingleton().destroyAnimation(animDefIt->second);
	}
	
	m_mapAnimDef.clear();
	m_vAnimations.clear();
	m_vPausedAnim.clear();

	CEGUI::WindowManager::getSingleton().destroyWindow(m_pMainWindow);
	m_pMainWindow = 0;
}

//-------------------------------------------------------------------------------------------
//	Private assignment operator. (By Pierre)
//-------------------------------------------------------------------------------------------
void EnemyBar::operator=(const EnemyBar& p_uiEnemyBar)
{
}

//-------------------------------------------------------------------------------------------
//	Return singleton pointer. (By Pierre)
//-------------------------------------------------------------------------------------------
EnemyBar* EnemyBar::GetSingletonPtr()
{
	if(!m_uiEnemyBar)
		m_uiEnemyBar = new EnemyBar();

	return m_uiEnemyBar;
}

//-------------------------------------------------------------------------------------------
//	Public destructor. (By Pierre)
//-------------------------------------------------------------------------------------------
void EnemyBar::Destroy()
{
	delete m_uiEnemyBar;
}

//-------------------------------------------------------------------------------------------
//	Initialize the in-game HUD. (By Pierre)
//-------------------------------------------------------------------------------------------
void EnemyBar::Init()
{
	if(m_bInitialized)
		return;

	// Create enemy progress bar
	m_pMainWindow = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "HUD_EnemyBar");
	m_pMainWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.39,0), CEGUI::UDim(1,0)));
	m_pMainWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.2,0), CEGUI::UDim(0,0)));
	m_pMainWindow->setProperty( "Image", "set:Essentials image:EnemyBar");

	m_bInitialized = true;
}

//-------------------------------------------------------------------------------------------
//	Move window to top. (By Pierre)
//-------------------------------------------------------------------------------------------
void EnemyBar::MoveToTop()
{
	BaseWindow::MoveToTop();
}

//-------------------------------------------------------------------------------------------
//	Push window to back. (By Pierre)
//-------------------------------------------------------------------------------------------
void EnemyBar::PushToBack()
{
	BaseWindow::PushToBack();
}

//-------------------------------------------------------------------------------------------
//	Create animation for enemy waves. (By Pierre)
//-------------------------------------------------------------------------------------------
void EnemyBar::CreateAnimation(int p_iEnemyType, int p_iEnemyCount, float p_fDuration, bool p_bFast)
{
	// Find animation definition
	std::map<float, CEGUI::Animation*>::iterator animationIt = m_mapAnimDef.find(p_fDuration);

	// Create an animation definition if it could find the same one
	if(animationIt == m_mapAnimDef.end())
	{
		CEGUI::Animation* animDef = CEGUI::AnimationManager::getSingleton().createAnimation(
									static_cast<std::ostringstream*>( &(std::ostringstream() << m_iAnimCount) )->str());
		m_iAnimCount++;
		
		animDef->setDuration((p_fDuration + 3.0f)); 
		animDef->setReplayMode(CEGUI::Animation::RM_Once);
		
		CEGUI::Affector* affector = animDef->createAffector("UnifiedXPosition", "UDim");
		affector->createKeyFrame(0.0f, "{1.0,0.0}");
		//affector->createKeyFrame(p_fDuration - 5.0f, "{0.95,0.0}");
		affector->createKeyFrame(p_fDuration - 5.0f, "{1.0,0.0}");
		affector->createKeyFrame(p_fDuration, "{0.245,0.0}");
		affector->createKeyFrame((p_fDuration + 3.0f), "{0.001,0.0}");

		affector = 0;

		affector = animDef->createAffector("Visible", "bool");
		affector->createKeyFrame(0.0f, "False");
		//affector->createKeyFrame(p_fDuration - 1.0f, "True");
		affector->createKeyFrame(p_fDuration, "True");
		affector->createKeyFrame((p_fDuration + 5.5f), "False");

		m_mapAnimDef.insert(std::pair<float, CEGUI::Animation*>(p_fDuration, animDef));

		animDef = 0;
		affector = 0;

		animationIt = m_mapAnimDef.find(p_fDuration);
	}

	// Create enemy icon
	CEGUI::Window* enemyWindow = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage",
									static_cast<std::ostringstream*>( &(std::ostringstream() << m_iWindowCount) )->str());
	m_iWindowCount++;

	enemyWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.05,0), CEGUI::UDim(0.7,0)));
	enemyWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.95,0), CEGUI::UDim(0.15,0)));
	//enemyWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.245,0), CEGUI::UDim(0.15,0)));
	
	// Different enemy icons for single/multiple enemies. Different enemy icons for different type, too.
	if(p_iEnemyCount == 1)
	{
		if(p_iEnemyType == 1)
		{
			(p_bFast)?enemyWindow->setProperty( "Image", "set:Essentials image:FastEasy"):
				enemyWindow->setProperty( "Image", "set:Essentials image:Easy");
		}

		else if(p_iEnemyType == 0)
		{
			(p_bFast)?enemyWindow->setProperty( "Image", "set:Essentials image:FastMedian"):
				enemyWindow->setProperty( "Image", "set:Essentials image:Median");
		}

		else if(p_iEnemyType == 3)
		{
			(p_bFast)?enemyWindow->setProperty( "Image", "set:Essentials image:FastHard"):
				enemyWindow->setProperty( "Image", "set:Essentials image:Hard");
		}

		else if(p_iEnemyType == 2)
		{
			(p_bFast)?enemyWindow->setProperty( "Image", "set:Essentials image:FastSpecial"):
				enemyWindow->setProperty( "Image", "set:Essentials image:Special");
		}
	}

	else if(p_iEnemyCount > 1)
	{
		if(p_iEnemyType == 1)
		{
			(p_bFast)?enemyWindow->setProperty( "Image", "set:Essentials image:MultiFastEasy"):
				enemyWindow->setProperty( "Image", "set:Essentials image:MultiEasy");
		}

		else if(p_iEnemyType == 0)
		{
			(p_bFast)?enemyWindow->setProperty( "Image", "set:Essentials image:MultiFastMedian"):
				enemyWindow->setProperty( "Image", "set:Essentials image:MultiMedian");
		}

		else if(p_iEnemyType == 3)
		{
			(p_bFast)?enemyWindow->setProperty( "Image", "set:Essentials image:MultiFastHard"):
				enemyWindow->setProperty( "Image", "set:Essentials image:MultiHard");
		}

		else if(p_iEnemyType == 2)
		{
			(p_bFast)?enemyWindow->setProperty( "Image", "set:Essentials image:MultiFastSpecial"):
				enemyWindow->setProperty( "Image", "set:Essentials image:MultiSpecial");
		}
	}

	enemyWindow->setVisible(false);
	m_pMainWindow->addChildWindow(enemyWindow);

	// Bind window with animation definition to make runnable animation
	CEGUI::AnimationInstance* animation = CEGUI::AnimationManager::getSingleton().instantiateAnimation(animationIt->second);
	animation->setTargetWindow(enemyWindow);

	m_vAnimations.push_back(std::pair<CEGUI::AnimationInstance*, CEGUI::Window*>(animation, enemyWindow));
}

//void EnemyBar::StartAnim(int p_iGroups)
//{
//	if(m_vAnimations.size() == m_iCurrentEnemyGroup)
//		return;
//
//	for(int i = m_iCurrentEnemyGroup; i < m_vAnimations.size(); ++i)
//		m_vAnimations[i].first->start();
//	
//	m_iCurrentEnemyGroup = m_vAnimations.size();
//}

void EnemyBar::StartAnimWithWave(int p_iGroup, int p_iGroupNum)
{
	m_iCurrentEnemyGroup = p_iGroup;

	SendWave(p_iGroupNum);
}

void EnemyBar::SendWave(int p_iGroupNum)
{
	if(m_vAnimations.size() == m_iCurrentEnemyGroup)
		return;

	for(int i = m_iCurrentEnemyGroup; i < (m_iCurrentEnemyGroup+p_iGroupNum); ++i)
		m_vAnimations[i].first->start();
	
	m_iCurrentEnemyGroup += p_iGroupNum;
}

void EnemyBar::Pause()
{
	for(int i = 0; i < m_vAnimations.size(); i++)
	{
		if(m_vAnimations[i].first->isRunning())
		{
			m_vAnimations[i].first->pause();
			m_vPausedAnim.push_back(m_vAnimations[i].first);
		}
	}
}

void EnemyBar::Restart()
{
	for(int i = 0; i < m_vPausedAnim.size(); i++)
		m_vPausedAnim[i]->unpause();

	m_vPausedAnim.clear();
}

void EnemyBar::SpeedUp()
{
	m_fSpeedFactor = 2;
	for(int i = 0; i < m_vAnimations.size(); i++)
		m_vAnimations[i].first->setSpeed(m_fSpeedFactor);
}

void EnemyBar::SlowDown()
{
	m_fSpeedFactor = 1;
	for(int i = 0; i < m_vAnimations.size(); i++)
		m_vAnimations[i].first->setSpeed(m_fSpeedFactor);
}

void EnemyBar::ResetAnim()
{
	m_iWindowCount = 0;

	for(int i = 0; i < m_vAnimations.size(); i++)
	{
		CEGUI::WindowManager::getSingleton().destroyWindow(m_vAnimations[i].second);
		CEGUI::AnimationManager::getSingleton().destroyAnimationInstance(m_vAnimations[i].first);
	}

	m_vAnimations.clear();
}

void EnemyBar::Reset()
{
	m_iCurrentEnemyGroup = 0;
	m_iAnimCount = 0;
	m_iWindowCount = 0;
	m_fSpeedFactor = 1.0f;

	for(int i = 0; i < m_vAnimations.size(); i++)
	{
		CEGUI::WindowManager::getSingleton().destroyWindow(m_vAnimations[i].second);
		CEGUI::AnimationManager::getSingleton().destroyAnimationInstance(m_vAnimations[i].first);
	}

	std::map<float, CEGUI::Animation*>::iterator animDefIt = m_mapAnimDef.begin();
	for( ; animDefIt != m_mapAnimDef.end(); animDefIt++)
	{
		CEGUI::AnimationManager::getSingleton().destroyAllInstancesOfAnimation(animDefIt->second); // Double check
		CEGUI::AnimationManager::getSingleton().destroyAnimation(animDefIt->second);
	}

	m_mapAnimDef.clear();
	m_vAnimations.clear();
	m_vPausedAnim.clear();
}