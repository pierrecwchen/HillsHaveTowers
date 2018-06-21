//-------------------------------------------------------------------------------------------
//	SelectableManager.cpp										Original Author: Pierre Chen
//
//	Please see note from SelectableManager.h
//-------------------------------------------------------------------------------------------

#include "SelectableManager.h"
#include <stack>

/************************************************************************
* Static variables (By Pierre)
*************************************************************************/
bool SelectableManager::m_bPause = false;
Selectable* SelectableManager::m_pCurrentPointingObj = 0;
Selectable* SelectableManager::m_pCurrentSelectedObj = 0;
std::vector<Selectable*> SelectableManager::m_vSelectables;
Ogre::Camera* SelectableManager::m_pCamera = 0;

/************************************************************************
* Defult constructor. (By Pierre)
*************************************************************************/
SelectableManager::SelectableManager()
{
}

/************************************************************************
* Defult destructor. (By Pierre)
*************************************************************************/
SelectableManager::~SelectableManager()
{
}

/************************************************************************
* Add a Selectable pointer to the vector. (By Pierre)
*************************************************************************/
void SelectableManager::Add(Selectable* p_pSelectable)
{
	m_vSelectables.push_back(p_pSelectable);
}

/************************************************************************
* Add a Selectable pointer vector to the vector. (By Pierre)
*************************************************************************/
void SelectableManager::AddAll(std::vector<Selectable*> p_vSelectables)
{
	for(int i = 0; i < p_vSelectables.size(); i++)
	{
		m_vSelectables.push_back(p_vSelectables[i]);
	}
}

/************************************************************************
* Remove a Selectable pointer from the vector. (By Pierre)
*************************************************************************/
void SelectableManager::Remove(Selectable* p_pSelectable)
{
	for(int i = 0; i < m_vSelectables.size(); i++)
	{
		if(m_vSelectables[i] == p_pSelectable)
		{
			if(m_vSelectables[i] == m_pCurrentPointingObj)
				m_pCurrentPointingObj = 0;
			if(m_vSelectables[i] == m_pCurrentSelectedObj)
				m_pCurrentSelectedObj = 0;
			m_vSelectables[i] = 0;
			m_vSelectables.erase(m_vSelectables.begin() + i);
		}
	}
}

/************************************************************************
* Remove all Selectable pointers from the vector by given type. (By Pierre)
*************************************************************************/
void SelectableManager::RemoveAll(const SelectableObjType& p_tSelectableObj)
{
	// Pointing/selected nothing if the current pointed/selected object is same type
	// as the objects which going to be removed.
	if(m_pCurrentPointingObj != 0 &&
		m_pCurrentPointingObj->GetSelectableObjType() == p_tSelectableObj)
	{
		m_pCurrentPointingObj = 0;
	}

	if(m_pCurrentSelectedObj != 0 &&
		m_pCurrentSelectedObj->GetSelectableObjType() == p_tSelectableObj)
	{
		m_pCurrentSelectedObj = 0;
	}

	std::stack<int> tempStack;

	if(!m_vSelectables.empty())
	{
		for(int i = 0; i < m_vSelectables.size(); i++)
		{
			if(m_vSelectables[i]->GetSelectableObjType() == p_tSelectableObj)
			{
				tempStack.push(i);
			}
		}
	}

	// Do this to avoid erasing the wrong element in vector.
	while(!tempStack.empty())
	{
		m_vSelectables.erase(m_vSelectables.begin() + tempStack.top());
		tempStack.pop();
	}
}

/************************************************************************
* Remove all Selectable pointers from the vector. (By Pierre)
*************************************************************************/
void SelectableManager::RemoveAll()
{
	m_pCurrentPointingObj = 0;
	m_pCurrentSelectedObj = 0;
	if(!m_vSelectables.empty())
	{
		m_vSelectables.clear();
	}
}

/************************************************************************
* Mouse picking algorithm. Find and update the current pointing
* object.(By Pierre)
*************************************************************************/
void SelectableManager::UpdateCurrentPointingObj(const OIS::MouseEvent& p_rArg)
{
	if(m_bPause)
	{
		m_pCurrentPointingObj = 0;
		return;
	}

	// Get the mouse from CEGUI
	CEGUI::MouseCursor* pMouse = CEGUI::MouseCursor::getSingletonPtr();

	// Create a ray depends on Camera. The parameters are normalised x and y corrdinate on screen.
	Ogre::Ray mMouseRay = m_pCamera->getCameraToViewportRay(pMouse->getPosition().d_x / float(p_rArg.state.width),
															pMouse->getPosition().d_y / float(p_rArg.state.height));

	// Pair<bool isTheRayIntersectsTheBoundingBox,  Ogre::Real distanceBetweenCameraAndBox>
	std::pair<bool, Ogre::Real> tempPair;
	int currentClosest = -1;
	Ogre::Real currentShortest = 0;

	// Check if there is any selectable object intersect with the ray, and determine which of them
	// has the shortest distance.
	for(int i = 0; i < m_vSelectables.size(); i++)
	{
		// Check not equal to zero for safety. And also check if it is selectable.
		if(m_vSelectables[i] != 0 && m_vSelectables[i]->IsSelectable())
		{
			tempPair = mMouseRay.intersects(m_vSelectables[i]->GetBoundingBox());
			if(tempPair.first)
			{
				if(currentClosest == -1)
				{
					currentClosest = i;
					currentShortest = tempPair.second;
				}
				
				else
				{
					if(tempPair.second < currentShortest)
					{
						currentClosest = i;
						currentShortest = tempPair.second;
					}
				}
			}
		}
	}

	// If find an object and it is different from the one we currently pointing
	if(currentClosest != -1 && m_pCurrentPointingObj != m_vSelectables[currentClosest])
	{
		// Disable to show "previous" object if it is NOT the one which is selected, and if it is not NULL
		if(m_pCurrentPointingObj != 0 && !m_pCurrentPointingObj->IsSelected())
		{
			m_pCurrentPointingObj->ShowObject(false);
		}

		// Change to current object
		m_pCurrentPointingObj = m_vSelectables[currentClosest];

		// Enable to show current object if it is NOT showing itself yet
		if(!m_pCurrentPointingObj->IsShow())
		{
			m_pCurrentPointingObj->ShowObject(true);
		}
	}

	// If pointing to empty space, set pointing to nothing
	else if(currentClosest == -1 && m_pCurrentPointingObj != 0)
	{
		if(!m_pCurrentPointingObj->IsSelected())
		{
			m_pCurrentPointingObj->ShowObject(false);
		}
		m_pCurrentPointingObj = 0;
	}

	//std::vector<PickingObj> pickingObjTemp(0);
	//std::pair<bool, Ogre::Real> tempPair;
	//std::vector<PickingObj>::iterator pickingObjItBegin = pickingObjTemp.begin();
	//std::vector<PickingObj>::iterator pickingObjItEnd = pickingObjTemp.end();

	//// Check if there is any selectable object intersect with the ray, and add all of them
	//// into the pickingObjTemp vector in sorted way.
	//for(int i = 0; i < m_vSelectables.size(); i++)
	//{
	//	// Check not equal to zero for safety. And also check if it is selectable.
	//	if(m_vSelectables[i] != 0 && m_vSelectables[i]->IsSelectable())
	//	{
	//		tempPair = mMouseRay.intersects(m_vSelectables[i]->GetBoundingBox());
	//		// If there is an intersection
	//		if(tempPair.first)
	//		{
	//			// If the picking obj vector is empty
	//			if(pickingObjTemp.empty())
	//			{
	//				// Just add it to the back of vector
	//				pickingObjTemp.push_back(PickingObj(tempPair.second,
	//										m_vSelectables[i]->GetSelectableObjType(),
	//										m_vSelectables[i]));
	//			}

	//			else
	//			{
	//				// If the distance is bigger than or equal to the one in the end of vector
	//				if(pickingObjItEnd->m_rDistance <= tempPair.second)
	//				{
	//					// Just add it to the back of vector
	//					pickingObjTemp.push_back(PickingObj(tempPair.second,
	//														m_vSelectables[i]->GetSelectableObjType(),
	//														m_vSelectables[i]));
	//				}

	//				else
	//				{
	//					// Otherwise, insert the object into sorted vector
	//					for(int j = 0; j < pickingObjTemp.size(); j++)
	//					{
	//						if(pickingObjTemp[j].m_rDistance > mMouseRay.intersects(m_vSelectables[i]->GetBoundingBox()).second)
	//						{
	//							pickingObjTemp.insert((pickingObjItBegin + j), PickingObj(tempPair.second,
	//																						m_vSelectables[i]->GetSelectableObjType(),
	//																						m_vSelectables[i]));
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	//if(!pickingObjTemp.empty())
	//{
	//	// If only one object is intersected
	//	if(pickingObjTemp.size() == 1)
	//	{
	//		// If the object is different from the one we currently pointing
	//		if(m_pCurrentPointingObj != pickingObjTemp.begin()->m_sEntity)
	//		{
	//			// Disable to show "previous" object if it is NOT the one which is selected, and if it is not NULL
	//			if(m_pCurrentPointingObj != 0 && !m_pCurrentPointingObj->IsSelected())
	//			{
	//				m_pCurrentPointingObj->ShowObject(false);
	//			}

	//			// Change to current object
	//			m_pCurrentPointingObj = pickingObjTemp.begin()->m_sEntity;

	//			// Enable to show current object if it is NOT showing itself yet
	//			if(!m_pCurrentPointingObj->IsShow())
	//			{
	//				m_pCurrentPointingObj->ShowObject(true);
	//			}
	//		}
	//	}

	//	else
	//	{
	//		// If the first one is tile
	//		if(pickingObjTemp.begin()->m_tSelectableObj == SO_Tile)
	//		{
	//			

	//			// If there is more than one object intersected
	//			else
	//			{
	//				for(int i = 0; i < pickingObjTemp.size(); i++)
	//				{
	//					if(pickingObjTemp[i]->m_tSelectableObj != SO_Tile)
	//					{

	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	//// If pointing to empty space, set pointing to nothing
	//else if(pickingObjTemp.empty() && m_pCurrentPointingObj != 0)
	//{
	//	// If the pointing object is not selected, show no bounding box.
	//	if(!m_pCurrentPointingObj->IsSelected())
	//	{
	//		m_pCurrentPointingObj->ShowObject(false);
	//	}

	//	m_pCurrentPointingObj = 0;
	//}

	//// If find an object and it is different from the one we currently pointing
	//if(currentClosest != -1 && m_pCurrentPointingObj != m_vSelectables[currentClosest])
	//{
	//	// Disable to show "previous" object if it is NOT the one which is selected, and if it is not NULL
	//	if(m_pCurrentPointingObj != 0 && !m_pCurrentPointingObj->IsSelected())
	//	{
	//		m_pCurrentPointingObj->ShowObject(false);
	//	}

	//	// Change to current object
	//	m_pCurrentPointingObj = m_vSelectables[currentClosest];

	//	// Enable to show current object if it is NOT showing itself yet
	//	if(!m_pCurrentPointingObj->IsShow())
	//	{
	//		m_pCurrentPointingObj->ShowObject(true);
	//	}
	//}
}

/************************************************************************
* Mouse picking algorithm. Find and update the current selected
* object.(By Pierre)
*************************************************************************/
void SelectableManager::UpdateCurrentSelectedObj(const OIS::MouseEvent& p_rArg)
{
	if(m_bPause)
	{
		m_pCurrentSelectedObj = 0;
		return;
	}

	//UpdateCurrentPointingObj(p_rArg);

	// If the mouse currently pointing something and is not the selected object.
	if(m_pCurrentPointingObj != 0 && m_pCurrentPointingObj != m_pCurrentSelectedObj)
	{
		// Disable to show the selected object and set "not selected" if the "previous" object is not NULL
		if(m_pCurrentSelectedObj != 0)
		{
			m_pCurrentSelectedObj->SetSelected(false);
			m_pCurrentSelectedObj->ShowObject(false);
		}

		// Change to current selected object
		m_pCurrentSelectedObj = m_pCurrentPointingObj;

		// Set it to be selected
		m_pCurrentSelectedObj->SetSelected(true);
	}

	// If the mouse currently pointing to nothing and there is a selected object.
	else if(m_pCurrentPointingObj == 0 && m_pCurrentSelectedObj != 0)
	{
		// Disable to show the selected object, set to "not selected", and selected nothing.
		m_pCurrentSelectedObj->SetSelected(false);
		m_pCurrentSelectedObj->ShowObject(false);
		m_pCurrentSelectedObj = 0;
	}


}