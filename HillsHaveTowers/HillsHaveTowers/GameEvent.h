//-------------------------------------------------------------------------------------------
//	GameEvent.h												Original Author: Pierre Chen
//
//	A class which stores essential attritubes for an event.
//-------------------------------------------------------------------------------------------

#ifndef _GAMEEVENT_
#define _GAMEEVENT_

#include <list>
#include "Tile.h"

namespace Hills
{

	enum GameEventType
	{
		GE_NULL = 0,
		GE_OpenNewPath,
		GE_AreaAttack,
        GE_SlowAttack,
		GE_Heal,
        GE_FreezeAttack,
        GE_BossIncoming,
        GE_DayNightChange,
		GE_NUMEVENT
	};

	enum AreaOfEffectType
	{
		AOE_NULL = 0,
		AOE_Cone,
		AOE_Sphere,
		AOE_NUMAOETYPE
	};
	
	class GameEvent
	{
	private:
		GameEventType m_GameEventType;
		AreaOfEffectType m_AOEType;
		
		// Area Of Effect & healing
		int m_iDirection, m_iAngle;  // Angle tower is facing and width of cone
		float m_fAmount; // Amount of healing or attacking
        float m_fRadius;
		Ogre::Vector3 m_vOrigin;
		Ogre::Vector3 m_vDirection;
        bool m_bIsNight;
        std::string m_sID;

		// Shortest path
		std::list<Tile> m_lShortestPath;
		
	public:
	
		GameEvent();
		GameEvent(GameEventType p_GameEventType, std::list<Tile>& p_lShortestPath);
		GameEvent(GameEventType p_GameEventType, AreaOfEffectType p_AOEType, Ogre::Vector3& p_vOrigin, float p_fRadius, int p_iAmount, int p_iDirection, int p_iAngle = 0);
        GameEvent(GameEventType p_GameEventType, Ogre::Vector3& p_vOrigin, float p_fRadius, float p_fPercent);  // Slow event (added by Keenan)
        GameEvent(GameEventType p_GameEventType, Ogre::Vector3& p_vOrigin, float p_fRadius); // Freeze event (added by Keenan)
		GameEvent(GameEventType p_GameEventType, Ogre::Vector3& p_vPriestLoc, int p_iAmountHealing, std::string p_sID);
        GameEvent(GameEventType p_GameEventType); //BossIncoming event
        GameEvent(GameEventType p_GameEventType, bool p_bIsNight); //DayNightChange event
		~GameEvent();
		
		inline const GameEventType GetGameEventType()const{ return m_GameEventType; }
		inline const AreaOfEffectType GetAreaOfEffectType()const{ return m_AOEType; }
		
		inline const int GetAngle()const{ return m_iAngle; }
        inline const int GetDirection()const{ return m_iDirection; }
		inline const float GetAmount()const{ return m_fAmount; }
        inline const float GetRadius()const { return m_fRadius; }
		inline const Ogre::Vector3& GetOrigin()const{ return m_vOrigin; }
        inline const std::string GetID()const { return m_sID; }
        inline const bool IsNight() const { return m_bIsNight; }

		inline const std::list<Tile>& GetShortestPath()const{ return m_lShortestPath; }
	};

}
#endif