//-----------------------------------------------------------------------------
// SelectionCircle.h                                        By: Keenan Johnston
// 
// Represents a variety of selection circle types to be displayed as indicators
// on the terrain during play.  They are implemented as static because only
// one instance of each will ever exist, and are drawn using projected textures
// so that they conform to the terrain they are displayed on.
//-----------------------------------------------------------------------------

#ifndef SELECTIONCIRCLE_H
#define SELECTIONCIRCLE_H

#include "BaseApplication.h"
#include "Terrain.h"
#include "Selectable.h"

class SelectionCircle
{
    public:
        // Cleanup method
        static void Destroy();

        // Set up the projection frustums
        static void Initialize(std::string p_sTextureName);

        // Move the circles between objects
        static void LockTo(Ogre::SceneNode* p_pSceneNode, float p_fSize = 10.0f, float p_fOffset = 0.0f);
        static void HoverOver(Ogre::SceneNode* p_pSceneNode, float p_fSize = 10.0f, float p_fOffset = 0.0f);
        static void LockTarget(Ogre::SceneNode* p_pSceneNode, float p_fSize = 10.0f, float p_fOffset = 0.0f);

        // Method for a specific object to get rid of any circles on it
        static void Deselect(Ogre::SceneNode* p_pSceneNode);

        // General methods to hide circles
        static void HideSelectionCircle();
        static void HideHoverCircle();
        static void HideTargetCircle();

        // Update the position of the selection circle in case the object is moving
        static void Update();

    private:
        // Member variables
        static Selectable* m_pSelectedObject;
        static Ogre::SceneNode* m_pSelectionNode, *m_pLockNode;
        static Ogre::SceneNode* m_pHoverNode, *m_pHoverLockNode;
        static Ogre::SceneNode* m_pTargetNode, *m_pTargetLockNode;
        static Ogre::Frustum* m_pSelectionFrustum, *m_pHoverFrustum, *m_pTargetFrustum;
        static bool m_bSelectionCircleVisible;
        static float m_fSelectionSize, m_fHoverSize, m_fTargetSize;
        static float m_fSelectionOffset, m_fHoverOffset, m_fTargetOffset;
};

#endif