//-------------------------------------------------------------------------------------------
//	EventManager.cpp											Original Author: Pierre Chen
//
//	Please see EventManager.h for detail.
//-------------------------------------------------------------------------------------------

#include "EventManager.h"

namespace Hills
{
	std::map<GameEventType, std::vector<Recipient*>> EventManager::m_mapRecipients;
	std::queue<GameEvent> EventManager::m_qGameEvents;

	//------------------------------------------------------------
	// Private defult constructor (By Pierre)
	//------------------------------------------------------------
	EventManager::EventManager()
	{
	}

	//------------------------------------------------------------
	// Private destructor (By Pierre)
	//------------------------------------------------------------
	EventManager::~EventManager()
	{
	}

	//------------------------------------------------------------
	// Add an event to GameEvent vector (By Pierre)
	//------------------------------------------------------------
	void EventManager::AddEvent(const GameEvent& p_geEvent)
	{
		m_qGameEvents.push(p_geEvent);
	}

	//------------------------------------------------------------
	// Remove all events from GameEvent vector (By Pierre)
	//------------------------------------------------------------
	void EventManager::RemoveAllEvent()
	{
		while(!m_qGameEvents.empty())
		{
			m_qGameEvents.pop();
		}
	}

	//------------------------------------------------------------
	// Add recipient to the member vector (By Pierre)
	//------------------------------------------------------------
	void EventManager::AddRecipient(Recipient* p_pRecipient, int p_iEventNum, ...)
	{
		va_list arguments;
		va_start(arguments, p_iEventNum);

		for(int i = 0; i < p_iEventNum; i++)
			m_mapRecipients[va_arg(arguments, GameEventType)].push_back(p_pRecipient);

		va_end(arguments);
	}

	//------------------------------------------------------------
	// Remove a recipient from the map (By Pierre)
	//------------------------------------------------------------
	void EventManager::RemoveRecipient(Recipient* p_pRecipient)
	{
		std::map<GameEventType, std::vector<Recipient*>>::iterator recipientIt = m_mapRecipients.begin();

		for(; recipientIt != m_mapRecipients.end(); recipientIt++)
			for(int i = 0; i < recipientIt->second.size(); ++i)
				if(recipientIt->second[i] == p_pRecipient)
					recipientIt->second.erase(recipientIt->second.begin()+i);
	}

	//----------------------------------------------------------------
	// Remove all recipients from the map by given type (By Pierre)
	//----------------------------------------------------------------
	void EventManager::RemoveAllRecipients(const Hills::RecipientType& p_tRecipientType)
	{
		std::map<GameEventType, std::vector<Recipient*>>::iterator recipientIt = m_mapRecipients.begin();

		for(; recipientIt != m_mapRecipients.end(); recipientIt++)
			for(int i = recipientIt->second.size() - 1; i >= 0; --i)
				if(recipientIt->second[i]->GetRecipientType() == p_tRecipientType)
					recipientIt->second.erase(recipientIt->second.begin()+i);
	}

	//------------------------------------------------------------
	// Remove all recipients from the vector (By Pierre)
	//------------------------------------------------------------
	void EventManager::RemoveAllRecipients()
	{
		std::map<GameEventType, std::vector<Recipient*>>::iterator recipientIt = m_mapRecipients.begin();

		for(; recipientIt != m_mapRecipients.end(); recipientIt++)
			recipientIt->second.clear();

		m_mapRecipients.clear();
	}

	//------------------------------------------------------------
	// Broad cast events in queue to all recipients in vector
	// (By Pierre)
	//------------------------------------------------------------
	void EventManager::BroadCast()
	{
		// Broadcast all the rest event(s).
		while(!m_qGameEvents.empty())
		{
			std::map<GameEventType, std::vector<Recipient*>>::iterator recipientIt = m_mapRecipients.find(m_qGameEvents.front().GetGameEventType());

			if(recipientIt != m_mapRecipients.end())
				for(int i = 0; i < recipientIt->second.size(); ++i)
					if(recipientIt->second[i]->IsAttached())
						recipientIt->second[i]->HandleEvent(m_qGameEvents.front());

			// Delete the first event in the queue.
			m_qGameEvents.pop();
		}
	}
}