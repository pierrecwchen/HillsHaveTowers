//-----------------------------------------------------------------------------
// BoundingBox.cpp                                          By: Keenan Johnston
// 
// Implementation file for BoundingBox.h
//-----------------------------------------------------------------------------

#include "BoundingBox.h"

//-----------------------------------------------------------------------------
// Create a simple wireframe box centered on the origin and return it to the
// caller, who can then transform it as required.
//-----------------------------------------------------------------------------
Ogre::ManualObject* BoundingBox::GenerateBoundingBox(Ogre::String p_sName, BoxTypes p_eType)
{
    Ogre::ManualObject* pCube = Ogre::Root::getSingletonPtr()->
                                getSceneManager(BaseApplication::GetSceneManagerName())->
                                createManualObject(p_sName);
	pCube->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
	// line 1
	pCube->position(-5, 0, -5);
	pCube->position(5, 0, -5);
	// line 2
	pCube->position(5, 10, -5);
	pCube->position(-5, 10, -5);
	// line 3
	pCube->position(-5, 0, -5);
	pCube->position(-5, 10, -5);
	// line 4
	pCube->position(5, 0, -5);
	pCube->position(5, 10, -5);
	// line 5
	pCube->position(-5, 0, 5);
	pCube->position(5, 0, 5);
	// line 6
	pCube->position(-5, 10, 5);
	pCube->position(5, 10, 5);
	// line 7
	pCube->position(-5, 0, 5);
	pCube->position(-5, 10, 5);
	// line 8
	pCube->position(5, 0, 5);
	pCube->position(5, 10, 5);
	// line 9
	pCube->position(-5, 0, -5);
	pCube->position(-5, 0, 5);
	// line 10
	pCube->position(5, 0, -5);
	pCube->position(5, 0, 5);
	// line 11
	pCube->position(-5, 10, -5);
	pCube->position(-5, 10, 5);
	// line 12
	pCube->position(5, 10, -5);
	pCube->position(5, 10, 5);

	pCube->end();
    pCube->setQueryFlags(p_eType);
    pCube->setCastShadows(false);

    return pCube;
}