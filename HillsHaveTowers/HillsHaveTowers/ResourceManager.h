//-----------------------------------------------------------------------------
// ResourceManager.h                                        By: Keenan Johnston
// 
// Manager class for resources.  Handles creating, updating and destroying
// resources as required.
//
// All code by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <vector>
#include <iostream>

#include "Wood.h"
#include "Stone.h"
#include "Iron.h"

#include "Tile.h"
#include "SelectableManager.h"
#include "EventManager.h"

enum Resources {WoodResource, StoneResource, IronResource};

class ResourceManager
{
    public:
        friend class BaseResource;

        // Add and delete methods
        static BaseResource* Add(Resources p_eType, Tile* p_pTile, int p_iAmount);
        static void Delete(BaseResource* p_pResource);
        static void Delete(int p_iX, int p_iY); // testing method, will be removed later
        static void DeleteAll();

        // Update all resources
        static void Update(const Ogre::FrameEvent& p_feEvent);

        // Getter methods
		//static Resource* Get(/*something*/);
		static inline std::vector<BaseResource*> GetAll() { return m_vResources; }
		static inline int Size() { return m_vResources.size(); }

    private:
        // All resources currently on the field
        static std::vector<BaseResource*> m_vResources;
};

#endif