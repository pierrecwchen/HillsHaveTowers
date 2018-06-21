// Created by Pavel Szczesny
#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include "BaseState.h"
#include <string>
#include <map>

namespace Hills
{
	class StateManager
	{
	public:
		StateManager();
		~StateManager();

		static void Update(const Ogre::FrameEvent& p_evtDelta);
		static BaseState* GetCurrentState() { return m_pCurrentState; }
		static void RegisterState(const std::string& p_strName, BaseState* p_pState);
		static void SwitchState(const std::string& p_strName);

		static void Destroy();

	private:
		static BaseState* m_pCurrentState;
		static BaseState* m_pPendingState;
		static std::map<std::string, BaseState*> m_mapRegisteredStates;
	};
}
#endif