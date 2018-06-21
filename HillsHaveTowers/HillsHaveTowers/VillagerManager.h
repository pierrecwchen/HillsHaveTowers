//-----------------------------------------------------------------------------
// VillagerManager.h                                        By: Keenan Johnston
// 
// Manager class for villagers.  Handles creating, updating and deleting
// villagers as required.  Includes some utility methods for collision
// detection so enemies can pick up villagers.
//
// All code by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef VILLAGERMANAGER_H
#define VILLAGERMANAGER_H

#include <vector>
#include <iostream>

#include "Village.h"
#include "Villager.h"
#include "Tile.h"
#include "TileSet.h"

class VillagerManager
{
    public:
        friend class Villager;
        
        static void CreateVillage();

        // Add and delete methods
        static Villager* Add(Ogre::Vector3 p_v3Loc, Tile p_tStartTile, std::list<Tile> p_lPath);
        static void Delete(Villager* p_vVillager);
		static void DeleteAll();

        //NICK~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
        //Interaction with the village
        static int RemoveFromVillage(int p_iVillagers);
        static inline int GetVillagerCount() { return Village::Instance()->GetPopulation(); }
        static inline void VillagersStolen(int p_iStolen) { m_iVillagersStolen += p_iStolen; }
        static inline int GetNumVillagersStolen() { return m_iVillagersStolen; }
        //~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*

        // Update all villagers
        static void Update(const Ogre::FrameEvent& p_feEvent);

        // Getter methods
		//static Villager* Get(/*something*/);
		static inline std::vector<Villager*> GetAll() { return m_vVillagers; }
		static inline int Size() { return m_vVillagers.size(); }

        // Collision detection
		static bool CollisionCheck(Ogre::Vector3 p_v3Loc);
        static Villager* GetCollidedVillager(Ogre::Vector3 p_v3Loc);


    private:
        static int m_iVillagersStolen;
        // All villagers currently on the field, their path, and the village
        static std::vector<Villager*> m_vVillagers;
        static std::list<Tile> m_lShortestPath;
        static bool m_bVillageCreated;
};

#endif