//-------------------------------------------------------------------------------------------
//	Selectable.cpp											Original Author: Pierre Chen
//
//	Please see note from Selectable.h.
//-------------------------------------------------------------------------------------------

#include "Selectable.h"

/************************************************************************
* Defult constructor. The object is not selectable in the beginning.
* (By Pierre)
*************************************************************************/
Selectable::Selectable():
m_bIsSelectable(true),
m_bIsSelected(false),
m_bIsShow(false),
m_tSelectableObj(SO_NULL)
{
}

/************************************************************************
* Defult destructor. (By Pierre)
*************************************************************************/
Selectable::~Selectable()
{
}