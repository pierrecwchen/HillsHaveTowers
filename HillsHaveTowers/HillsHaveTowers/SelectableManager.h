//-------------------------------------------------------------------------------------------
//	SelectableManager.h											Original Author: Pierre Chen
//
//	A class which stores a vector of selectable object, current pointing object, and current
//	selected object. Provides basic functions to deal with the vector and getters. Also
//	provides a function which provide the mouse picking algorithm.
//-------------------------------------------------------------------------------------------

#ifndef _SELECTABLEMANAGER_
#define _SELECTABLEMANAGER_

#include <vector>
#include "Selectable.h"
#include "OgreRay.h"
#include "OgreCamera.h"
#include "CEGUIMouseCursor.h"
#include "OISMouse.h"

//struct PickingObj
//{
//	Ogre::Real m_rDistance;
//	SelectableObjType m_tSelectableObj;
//	Selectable* m_sEntity;
//
//	PickingObj(): m_rDistance(0), m_sEntity(0), m_tSelectableObj(SO_NULL){}
//	
//	PickingObj(const PickingObj& p_prPickingObj):
//		m_rDistance(p_prPickingObj.m_rDistance),
//		m_sEntity(p_prPickingObj.m_sEntity),
//		m_tSelectableObj(p_prPickingObj.m_tSelectableObj){}
//	
//	PickingObj(Ogre::Real p_rDistance, SelectableObjType p_tSelectableObj, Selectable* p_sEntity):
//		m_rDistance(p_rDistance), m_sEntity(p_sEntity), m_tSelectableObj(p_tSelectableObj){}
//	
//	~PickingObj(){ m_sEntity = 0; }
//};

class SelectableManager
{
private:
	static bool m_bPause;
	static Selectable* m_pCurrentPointingObj;
	static Selectable* m_pCurrentSelectedObj;
	static std::vector<Selectable*> m_vSelectables;
	static Ogre::Camera* m_pCamera;

public:
	SelectableManager();
	~SelectableManager();

	static void Add(Selectable* p_pSelectable);
	static void AddAll(std::vector<Selectable*> p_vSelectables);
	static void Remove(Selectable* p_pSelectable);
	static void RemoveAll(const SelectableObjType& p_tSelectableObj);
	static void RemoveAll();
    //static Selectable* GetSelectable();
	static inline void SetPause(bool p_bPause = true){ m_bPause = p_bPause; }
	static inline Selectable* GetCurrentPointingObj() { return m_pCurrentPointingObj; }
	static inline Selectable* GetCurrentSelectedObj() { return m_pCurrentSelectedObj; }
	static inline void SetCamera(Ogre::Camera* p_pCamera) { m_pCamera = p_pCamera; }
	static inline Ogre::Camera* GetCamera() { return m_pCamera; }
	static void UpdateCurrentPointingObj(const OIS::MouseEvent& p_rArg);
	static void UpdateCurrentSelectedObj(const OIS::MouseEvent& p_rArg);
};

#endif