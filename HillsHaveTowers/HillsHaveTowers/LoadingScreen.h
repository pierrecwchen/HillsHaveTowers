//-------------------------------------------------------------------------------------------
//	LoadingScreen.h											Original Author: Pierre Chen
//
//	A singleton class which give the functionalities for loading screen
//-------------------------------------------------------------------------------------------

#ifndef _LOADINGSCREEN_
#define _LOADINGSCREEN_

#include "BaseWindow.h"

class LoadingScreen: public BaseWindow
{
private:
	static LoadingScreen* m_lsLoadingScreen;

	LoadingScreen();
	LoadingScreen(const LoadingScreen& p_lsLoadingScreen);
	~LoadingScreen();
	
	void operator=(const LoadingScreen& p_lsLoadingScreen);

public:
	static LoadingScreen* GetSingletonPtr();
	static void Destroy();

	virtual void Init();
	virtual void MoveToTop();
	virtual void PushToBack();

	void IncreaseProgressBar(float p_fStepSize);
	void IncreaseProgressBarTo(float p_fProgressValue);
	void ResetProgressBar();
};

#endif