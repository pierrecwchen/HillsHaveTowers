//-------------------------------------------------------------------------------------------
//	Recipient.h											Original Author: Pierre Chen
//
//	A parent class for all types of enemy, tower, tile, world, player, resource, villager.
//	Provide a bool variable which shows if it attach to eventManager or not. Methods
//	which can change the bool value, and an interface of handleEvent() method.
//
//	Remember: Subclasses should override the handleEvent() in their own function way.
//-------------------------------------------------------------------------------------------

#ifndef _RECIPIENT_
#define _RECIPIENT_

#include "GameEvent.h"

namespace Hills
{

	enum RecipientType
	{
		R_NULL = 0,
		R_Enemy,
		R_Tower,
		R_Resource,
		R_Villager,
		R_Player,
		R_NUMSOTYPE
	};

	class Recipient
	{
	private:
		
	protected:
		bool m_bIsAttached;
		RecipientType m_tRecipient;

		Recipient();
		~Recipient();
	
	public:
	
		// Temporary attach to the eventManager
		inline void Attach(){ m_bIsAttached = true; }
	
		// Temporary deattach from the eventManager
		inline void Deattach(){ m_bIsAttached = false; }
	
		inline bool IsAttached()const{ return m_bIsAttached; }

		inline const Hills::RecipientType& GetRecipientType()const{ return m_tRecipient; }
		inline void SetRecipientType(const Hills::RecipientType& p_tRecipient){ m_tRecipient = p_tRecipient; }
	
		// Handle event method, each subclass has their own way to take care of an event.
		virtual void HandleEvent(const GameEvent& p_geGameEvent) = 0; // Override it
	};

}
#endif