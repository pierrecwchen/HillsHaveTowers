//-------------------------------------------------------------------------------------------
//	Selectable.h											Original Author: Pierre Chen
//
//	A class which allows all entities and tiles to derive from. The class provides functions
//	which used to determine if the object is selectable and get the bounding box of the
//	object.
//-------------------------------------------------------------------------------------------

#ifndef _SELECTABLE_
#define _SELECTABLE_

#include "OgreAxisAlignedBox.h"

enum SelectableObjType
{
	SO_NULL = 0,
	SO_Enemy,
	SO_Tower,
	SO_Resource,
	SO_Villager,
	SO_Tile,
	SO_Player,
	SO_NUMSOTYPE
};

class Selectable
{
protected:
	bool m_bIsSelectable;
	bool m_bIsSelected;
	bool m_bIsShow;
	SelectableObjType m_tSelectableObj;

	Selectable();
	~Selectable();

public:
	virtual const Ogre::AxisAlignedBox& GetBoundingBox() = 0; // Override it
	virtual void ShowObject(bool p_bIsShow) = 0; // Override it
    virtual Ogre::Vector3 GetLocation() = 0; // Override it
	inline bool IsSelectable() { return m_bIsSelectable; }
	inline bool IsSelected() { return m_bIsSelected; }
	inline bool IsShow() { return m_bIsShow; }
	inline void SetSelectable(bool p_bIsSelectable) { m_bIsSelectable = p_bIsSelectable; }
	inline void SetSelected(bool p_bIsSelected) { m_bIsSelected = p_bIsSelected; }
	inline const SelectableObjType& GetSelectableObjType(){ return m_tSelectableObj; }
	inline void SetSelectableObjType(const SelectableObjType& p_tSelectableObj){ m_tSelectableObj = p_tSelectableObj; }
};

#endif