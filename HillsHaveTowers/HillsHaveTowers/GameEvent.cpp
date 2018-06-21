//-------------------------------------------------------------------------------------------
//	GameEvent.cpp											Original Author: Pierre Chen
//
//	Please see the note in GameEvent.h
//-------------------------------------------------------------------------------------------

#include "GameEvent.h"

namespace Hills
{
	//------------------------------------------------------------
	// Defult constructor (By Pierre)
	//------------------------------------------------------------
	GameEvent::GameEvent():
		m_GameEventType(GE_NULL),
		m_AOEType(AOE_NULL),
		m_iAngle(0),
		m_fAmount(0),
		m_vOrigin(Ogre::Vector3(0,0,0)),
		m_vDirection(Ogre::Vector3(0,0,0)),
		m_lShortestPath(std::list<Tile>())
	{
	}

	//------------------------------------------------------------
	// Constructor for OpenNewPath Event (By Pierre)
	//------------------------------------------------------------
	GameEvent::GameEvent(GameEventType p_GameEventType, std::list<Tile>& p_lShortestPath):
		m_GameEventType(p_GameEventType),
		m_AOEType(AOE_NULL),
		m_iAngle(0),
		m_fAmount(0),
		m_vOrigin(Ogre::Vector3(0,0,0)),
		m_vDirection(Ogre::Vector3(0,0,0)),
		m_lShortestPath(p_lShortestPath)
	{
	}
	
	//------------------------------------------------------------
	// Constructor for Area Of Effect Event (By Pierre)
	//------------------------------------------------------------
	GameEvent::GameEvent(GameEventType p_GameEventType, AreaOfEffectType p_AOEType, Ogre::Vector3& p_vOrigin, float p_fRadius, int p_iAmount, int p_iDirection, int p_iAngle):
		m_GameEventType(p_GameEventType),
		m_AOEType(p_AOEType),
		m_iAngle(p_iAngle),
        m_iDirection(p_iDirection),
		m_fAmount(p_iAmount),
		m_vOrigin(p_vOrigin),
		m_fRadius(p_fRadius),
		m_lShortestPath(std::list<Tile>())
	{
	}

    //------------------------------------------------------------
	// Constructor for Area Of Effect Slow Event (Added by Keenan)
	//------------------------------------------------------------
	GameEvent::GameEvent(GameEventType p_GameEventType, Ogre::Vector3& p_vOrigin, float p_fRadius, float p_fPercent):
		m_GameEventType(p_GameEventType),
		m_AOEType(AOE_Sphere),
		m_iAngle(0),
		m_fAmount(p_fPercent),
		m_vOrigin(p_vOrigin),
        m_fRadius(p_fRadius),
		m_vDirection(Ogre::Vector3(0,0,0)),
		m_lShortestPath(std::list<Tile>())
	{
	}

    //------------------------------------------------------------
	// Constructor for Area Of Effect Freeze Event (Added by Keenan)
	//------------------------------------------------------------
	GameEvent::GameEvent(GameEventType p_GameEventType, Ogre::Vector3& p_vOrigin, float p_fRadius):
		m_GameEventType(p_GameEventType),
		m_AOEType(AOE_Sphere),
		m_iAngle(0),
		m_fAmount(0),
		m_vOrigin(p_vOrigin),
        m_fRadius(p_fRadius),
		m_vDirection(Ogre::Vector3(0,0,0)),
		m_lShortestPath(std::list<Tile>())
	{
	}

	//------------------------------------------------------------
	// Constructor for Area Of Effect Event (By Pierre)
	//------------------------------------------------------------
	GameEvent::GameEvent(GameEventType p_GameEventType, Ogre::Vector3& p_vPriestLoc, int p_iAmountHealing, std::string p_sID):
		m_GameEventType(p_GameEventType),
		m_AOEType(AOE_NULL),
		m_iAngle(0),
		m_fAmount(p_iAmountHealing),
		m_vOrigin(p_vPriestLoc),
		m_vDirection(Ogre::Vector3(0,0,0)),
		m_lShortestPath(std::list<Tile>()),
        m_sID(p_sID)
	{
	}

        GameEvent::GameEvent(GameEventType p_GameEventType):
		m_GameEventType(p_GameEventType)
	{
	}

        GameEvent::GameEvent(GameEventType p_GameEventType, bool p_bIsNight):
		m_GameEventType(p_GameEventType),
        m_bIsNight(p_bIsNight)
	{
	}
	
	//------------------------------------------------------------
	// Destructor (By Pierre)
	//------------------------------------------------------------
	GameEvent::~GameEvent()
	{
		m_lShortestPath.clear();
	}
}