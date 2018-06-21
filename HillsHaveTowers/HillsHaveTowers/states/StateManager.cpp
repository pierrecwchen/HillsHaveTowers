//---------------------------------------------------------------------------
// StateManager.cpp									Created by Pavel Szczesny
//
// Main entry point for our game. This class allows us to switch to different
// states of our game.
//---------------------------------------------------------------------------
#include "StateManager.h"
#include <iostream>

namespace Hills
{
	std::map<std::string, BaseState*> StateManager::m_mapRegisteredStates;
	BaseState* StateManager::m_pCurrentState = 0;
	BaseState* StateManager::m_pPendingState = 0;
	
	/************************************************************************
	Constructor
	************************************************************************/
	StateManager::StateManager()
	{

	}

	/************************************************************************
	Destructor
	************************************************************************/
	StateManager::~StateManager()
	{
		
	}

	/************************************************************************
	Update
	************************************************************************/
	void StateManager::Update(const Ogre::FrameEvent& p_evtDelta)
	{
		// If state change has been requested, then switch
		if (m_pPendingState != 0 && m_pPendingState != m_pCurrentState)
		{
			if(m_pCurrentState != 0)
				m_pCurrentState->Destroy();

			m_pCurrentState = m_pPendingState;
			m_pPendingState = 0;
			m_pCurrentState->Init();
		}

		m_pCurrentState->Update(p_evtDelta);
	}

	/************************************************************************
	RegisterState

	Add new states to the manager
	************************************************************************/
	void StateManager::RegisterState(const std::string& p_strName, BaseState* p_pState)
	{
		if (m_mapRegisteredStates.find(p_strName) == m_mapRegisteredStates.end())
		{
			m_mapRegisteredStates.insert(std::pair<std::string, BaseState*>(p_strName, p_pState));
		}
		else
		{
			std::cout << p_strName << " is already registered\n";
		}
	}

	/************************************************************************
	SwitchState

	Tells manager to change state on next update
	************************************************************************/
	void StateManager::SwitchState(const std::string& p_strName)
	{
		if (m_mapRegisteredStates.find(p_strName) != m_mapRegisteredStates.end())
		{
			m_pPendingState = m_mapRegisteredStates.find(p_strName)->second;
		}
		else
		{
			std::cout << p_strName << " is not a registered state\n";
		}
	}

	/************************************************************************
	Destroy
	************************************************************************/
	void StateManager::Destroy()
	{
		if(m_pCurrentState)
			m_pCurrentState->Destroy();

		m_pCurrentState = 0;
		m_pPendingState = 0;

		m_mapRegisteredStates.clear();
	}
}