//-------------------------------------------------------------------------------------------
//	LoadingScreen.cpp											Original Author: Pierre Chen
//
//	Please see the note from LoadingScreen.h
//-------------------------------------------------------------------------------------------

#include "LoadingScreen.h"
#include <OgreRoot.h>

LoadingScreen* LoadingScreen::m_lsLoadingScreen = 0;

//-------------------------------------------------------------------------------------------
//	Private default constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
LoadingScreen::LoadingScreen()
{
}

//-------------------------------------------------------------------------------------------
//	Private copy constructor. (By Pierre)
//-------------------------------------------------------------------------------------------
LoadingScreen::LoadingScreen(const LoadingScreen& p_lsLoadingScreen)
{
}

//-------------------------------------------------------------------------------------------
//	Private destructor. (By Pierre)
//-------------------------------------------------------------------------------------------
LoadingScreen::~LoadingScreen()
{
	m_pMainWindow = 0;
}

//-------------------------------------------------------------------------------------------
//	Private assignment operator. (By Pierre)
//-------------------------------------------------------------------------------------------
void LoadingScreen::operator=(const LoadingScreen& p_lsLoadingScreen)
{
}

//-------------------------------------------------------------------------------------------
//	Return singleton. (By Pierre)
//-------------------------------------------------------------------------------------------
LoadingScreen* LoadingScreen::GetSingletonPtr()
{
	if(m_lsLoadingScreen == 0)
	{
		m_lsLoadingScreen = new LoadingScreen();
	}

	return m_lsLoadingScreen;
}

void LoadingScreen::Destroy()
{
	delete m_lsLoadingScreen;
}

void LoadingScreen::Init()
{
	if(m_bInitialized)
		return;

	// Create loading screen
	m_pMainWindow = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "LoadingScreen");
	m_pMainWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	m_pMainWindow->setSize(CEGUI::UVector2(CEGUI::UDim(1,0), CEGUI::UDim(1,0)));
	m_pMainWindow->setProperty("Image", "set:LS_Background image:full_image");
	m_pMainWindow->setProperty("BackgroundEnabled", "true");

	// Create progress bar
	CEGUI::ProgressBar* progressBar = dynamic_cast<CEGUI::ProgressBar*>(CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ProgressBar", "LS_Bar"));
	progressBar->setSize(CEGUI::UVector2(CEGUI::UDim(0.7,0), CEGUI::UDim(0.1,0)));
	progressBar->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15,0), CEGUI::UDim(0.8,0)));
	progressBar->setProgress(0.0f);
	progressBar->setStepSize(0.01f);

	m_pMainWindow->addChildWindow(progressBar);

	CEGUI::System::getSingleton().getGUISheet()->addChildWindow(m_pMainWindow);

	m_bInitialized = true;

	PushToBack();
}

void LoadingScreen::MoveToTop()
{
	BaseWindow::MoveToTop();
}

void LoadingScreen::PushToBack()
{
	BaseWindow::PushToBack();
}

void LoadingScreen::IncreaseProgressBar(float p_fStepSize)
{
	if(dynamic_cast<CEGUI::ProgressBar*>(CEGUI::WindowManager::getSingleton().getWindow("LS_Bar")) != 0)
	{
		dynamic_cast<CEGUI::ProgressBar*>(CEGUI::WindowManager::getSingleton().getWindow("LS_Bar"))->adjustProgress(p_fStepSize);

		if(Ogre::Root::getSingletonPtr() != 0)
			Ogre::Root::getSingleton().renderOneFrame();
	}
}

void LoadingScreen::IncreaseProgressBarTo(float p_fProgressValue)
{
	if(dynamic_cast<CEGUI::ProgressBar*>(CEGUI::WindowManager::getSingleton().getWindow("LS_Bar")) != 0)
	{
		dynamic_cast<CEGUI::ProgressBar*>(CEGUI::WindowManager::getSingleton().getWindow("LS_Bar"))->setProgress(p_fProgressValue);

		if(Ogre::Root::getSingletonPtr() != 0)
			Ogre::Root::getSingleton().renderOneFrame();
	}
}

void LoadingScreen::ResetProgressBar()
{
	if(dynamic_cast<CEGUI::ProgressBar*>(CEGUI::WindowManager::getSingleton().getWindow("LS_Bar")) != 0)
	{
		dynamic_cast<CEGUI::ProgressBar*>(CEGUI::WindowManager::getSingleton().getWindow("LS_Bar"))->setProgress(0.0);
	}
}