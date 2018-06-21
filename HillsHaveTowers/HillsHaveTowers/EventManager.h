//-------------------------------------------------------------------------------------------
//	EventManager.h											Original Author: Pierre Chen
//
//	A singleton class which handles all events and broadcast them to all recipients that
//	registered to this event manager.
//-------------------------------------------------------------------------------------------

#ifndef _EVENTMANAGER_
#define _EVENTMANAGER_

#include <cstdarg>
#include <map>
#include <queue>
#include "GameEvent.h"
#include "Recipient.h"

namespace Hills
{

	class EventManager
	{
	private:
		static std::map<GameEventType, std::vector<Recipient*>> m_mapRecipients;
		static std::queue<GameEvent> m_qGameEvents;
		
	public:

		EventManager();
		~EventManager();
	
		static void AddEvent(const GameEvent& p_geEvent);
		static void RemoveAllEvent();

		static void AddRecipient(Recipient* p_pRecipient, int p_iEventNum, ...);
		static void RemoveRecipient(Recipient* p_pRecipient);
		static void RemoveAllRecipients(const Hills::RecipientType& p_tRecipientType);
		static void RemoveAllRecipients();
	
		static void BroadCast();
	
	};

}
#endif