//-----------------------------------------------------------------------------
// BoundingBox.h                                            By: Keenan Johnston
// 
// Utility class representing a simple rectangular box used to overwrite
// OGRE's default bounding boxes to better fit an entity for raycasting.
//
// Box creation code written by Pavel, compartmentalized into seperate class
// with a few modifications by Keenan.
//-----------------------------------------------------------------------------

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "BaseApplication.h"

// Query flags used for raycasting
enum BoxTypes
{
    BT_RESOURCE = 1<<0,
    BT_ENEMY = 1<<1,
    BT_PLAYER = 1<<2,
    BT_VILLAGER = 1<<3,
    BT_TOWER = 1<<4,
    BT_TERRAIN = 1<<5,
    BT_OTHER = 1<<6
};

class BoundingBox
{
    public:
        // Static method to generate a bounding box with a given name and query flag
        static Ogre::ManualObject* GenerateBoundingBox(Ogre::String p_sName, BoxTypes p_eType);
};

#endif