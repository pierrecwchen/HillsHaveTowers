//-----------------------------------------------------------------------------
// BaseResource.h                                           By: Keenan Johnston
// 
// Represents a resource on the map within the game.  Has a physical model and
// can be harvested by the player for a given type of resource.  Works in
// conjunction with the ResourceManager, which handles updating and deleting
// the resources as required.
//
// All code by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef BASERESOURCE_H
#define BASERESOURCE_H

#include "Recipient.h"
#include "GameEvent.h"
#include "Selectable.h"
#include "Model.h"
#include "BoundingBox.h"
#include "TileSet.h"
#include "EventManager.h"
#include "InGameHUD.h"
#include "Terrain.h"
#include "MainCurrency.h"
#include "SelectionCircle.h"

enum ResourceStates {eIdleState, eDepletedState};
enum ResourceTypes {RT_Wood, RT_Stone, RT_Iron};

class BaseResource : public Selectable, public Hills::Recipient
{
    public:
        // Harvest the resource, decreasing its size by one
        virtual void Harvest();

        // Check flags for a few values
        inline bool IsHarvestable() { return m_bIsHarvestable; }
        inline bool CanDelete() { return m_bCanDelete; }

        // Clean-up any OGRE-related data
        virtual void DestroyModel();
        
        // Getters for some useful values
        inline Ogre::SceneNode* GetSceneNode() { return m_pModel->GetSceneNode(); }
		inline Ogre::Vector3 GetLocation() { return m_pModel->GetLocation(); }
        inline Tile* GetTile() { return m_pTile; }
        virtual ResourceTypes GetType() = 0;

        // Handle selection by selection circle with a size parameter
        void Select();

        // Update the resource each frame based on its state
        virtual void Update(const Ogre::FrameEvent& p_feEvent);

        //inline void Scale(Ogre::Vector3 p_v3Scalar) { m_pModel->Scale(p_v3Scalar); }
        //inline void Translate(Ogre::Vector3 p_v3Translation) { m_pModel->Translate(p_v3Translation); }
        //inline void Rotate(int p_iRotation) { m_pModel->Rotate(p_iRotation); }
		
        /***********************************************************************************
		*	Added by Pierre
		************************************************************************************/
		inline int GetAmount()const { return m_fAmount; }

		virtual const Ogre::AxisAlignedBox& GetBoundingBox() { return m_pBoundingBox->_getWorldAABB(); }
		virtual void ShowObject(bool p_bIsShow);

		// Handle event method, each subclass has their own way to take care of an event.
		virtual void HandleEvent(const Hills::GameEvent& p_geGameEvent);
		/***********************************************************************************/

    protected:
        // Resource related variables
        int m_fAmount;
        float m_fTimePerHarvest;
        bool m_bIsHarvestable;
        float m_fScale;

        // State and type enum values
        ResourceStates m_eState;
        ResourceTypes m_eType;

        // Tile this resource is on
        Tile* m_pTile;

        // Selection circle values
        float m_fCircleSize, m_fCircleOffset;

        // Death and deletion values
		bool m_bCanDelete;
        float m_fTimeUntilDeath;

        // Ogre related variables
        Model* m_pModel;
        Ogre::SceneNode* m_pBoundingBox;

        // Constructor and destructor
        BaseResource();
        ~BaseResource();
};

#endif
