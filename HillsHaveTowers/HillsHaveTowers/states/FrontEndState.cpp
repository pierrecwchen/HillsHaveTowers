/************************************************************************
FrontEndState.cpp								Created by Pavel Szczesny

State class for the main menu. While created by Pavel, the class
has been filled out by...
************************************************************************/

#include "FrontEndState.h"
#include <OgreRoot.h>
#include "../GameDataManager.h"

namespace Hills
{
	/************************************************************************
	Constructor
	************************************************************************/
	FrontEndState::FrontEndState()
	{
	
	}

	/************************************************************************
	Destructor
	************************************************************************/
	FrontEndState::~FrontEndState()
	{
		
	}

	/************************************************************************
	Init
	************************************************************************/
	void FrontEndState::Init()
	{
		m_bInitializing = true;

		UIManager::SetupResource();
		UIManager::SetupImageset();
		UIManager::UIInit();

		GameDataManager::GetSingletonPtr()->Init();

		UIManager::SwitchWindows("MainMenu");

		m_bInitializing = false;
	}

	/************************************************************************
	Update
	************************************************************************/
	void FrontEndState::Update(const Ogre::FrameEvent& p_evtDelta)
	{
		if(m_bInitializing)
			return;
	}

	/************************************************************************
	Destroy
	************************************************************************/
	void FrontEndState::Destroy()
	{

	}

	/************************************************************************
	KeyPressed
	************************************************************************/
	bool FrontEndState::KeyPressed( const OIS::KeyEvent& pArg )
	{
		return true;
	}

	/************************************************************************
	KeyReleased
	************************************************************************/
	bool FrontEndState::KeyReleased( const OIS::KeyEvent& pArg )
	{
		return true;
	}

	/************************************************************************
	MouseMoved
	************************************************************************/
	bool FrontEndState::MouseMoved( const OIS::MouseEvent& pArg )
	{
		return true;
	}

	/************************************************************************
	MousePressed
	************************************************************************/
	bool FrontEndState::MousePressed( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId )
	{
		return true;
	}

	/************************************************************************
	MouseReleased
	************************************************************************/
	bool FrontEndState::MouseReleased( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId )
	{
		return true;
	}
}