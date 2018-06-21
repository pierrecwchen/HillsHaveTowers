//-----------------------------------------------------------------------------
// Villager.h                                               By: Keenan Johnston
// 
// Represents a villager within the game.  Are found moving between the village
// and dropped on the path when an enemy is killed carrying one, at which point
// they will walk backwards along it to reach the village.
//
// Base code by Keenan, pathfinding by Nick, anything else as noted.
//-----------------------------------------------------------------------------

#ifndef VILLAGER_H
#define VILLAGER_H

#include <iostream>

#include "Model.h"
#include "Recipient.h"
#include "GameEvent.h"
#include "Selectable.h"

enum VillagerStates {eWalkState};

class Villager: public Hills::Recipient
{
    public:
        friend class VillagerManager;

        // Set the path for the villager to take to the village
        void SetShortestPath(std::list<Tile> p_lPath);

        // Deletion flag check
        inline bool CanDelete() { return m_bCanDelete; }

        // Villager picked up by enemy, can be removed from map
        inline void PickUp() { m_bCanDelete = true; }

        // Clean up OGRE-related data here
        void DestroyModel();

        // Getters for some useful information
        inline Ogre::SceneNode* GetSceneNode() { return m_pModel->GetSceneNode(); }
		inline Ogre::Vector3 GetLocation() { return m_pModel->GetLocation(); }
        inline VillagerStates GetState() { return m_eState; }

        // Update the villager each frame based on their state
		virtual void Update(const Ogre::FrameEvent& p_feEvent);

        //inline void Scale(Ogre::Vector3 p_v3Scalar) { m_pModel->Scale(p_v3Scalar); }
        //inline void Translate(Ogre::Vector3 p_v3Translation) { m_pModel->Translate(p_v3Translation); }
        //inline void Rotate(int p_iRotation) { m_pModel->Rotate(p_iRotation); }

		/***********************************************************************************
		*	Added by Pierre
		************************************************************************************/
		// Handle event method, each subclass has their own way to take care of an event.
		virtual void HandleEvent(const Hills::GameEvent& p_geGameEvent);
		/***********************************************************************************/

    private:
        // Villager related variables
		float m_fWalkSpeed;
        int m_iHeight;
        
        VillagerStates m_eState;
        bool m_bCanDelete;

        float m_fXOffset, m_fZOffset;

        // Path related variables
        std::list<Tile> m_lCurrentPath;
        std::list<Tile> m_lReceivedPath;

        float GetAngleDifference(float p_fCurrent, float p_fDestination);
        float GetAngleToNextTile();

        Tile m_tCurrentTile;

        bool m_bIsOnCorrectPath;

        void PlayReturnSound();
		
		// Ogre related variables
        Model* m_pModel;

        // Private constructor so class can not be initialized normally
        Villager(Ogre::Vector3 p_v3Loc, Tile p_tStartTile, std::list<Tile> p_lShortestPath);
        ~Villager();

        // Helper method to determine if villager is on new path
        bool IsOnNewPath();

};

#endif