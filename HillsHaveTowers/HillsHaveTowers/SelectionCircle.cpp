//-----------------------------------------------------------------------------
// SelectionCircle.cpp                                      By: Keenan Johnston
// 
// Implementation file for SelectionCircle.h
//-----------------------------------------------------------------------------

#include "SelectionCircle.h"

#include "LevelParser.h"

Selectable* SelectionCircle::m_pSelectedObject;

Ogre::SceneNode* SelectionCircle::m_pSelectionNode = 0;
Ogre::SceneNode* SelectionCircle::m_pLockNode = 0;

Ogre::SceneNode* SelectionCircle::m_pHoverNode = 0;
Ogre::SceneNode* SelectionCircle::m_pHoverLockNode = 0;

Ogre::SceneNode* SelectionCircle::m_pTargetNode = 0;
Ogre::SceneNode* SelectionCircle::m_pTargetLockNode = 0;

Ogre::Frustum* SelectionCircle::m_pSelectionFrustum = 0;
Ogre::Frustum* SelectionCircle::m_pHoverFrustum = 0;
Ogre::Frustum* SelectionCircle::m_pTargetFrustum = 0;

bool SelectionCircle::m_bSelectionCircleVisible = false;

float SelectionCircle::m_fSelectionSize = 0;
float SelectionCircle::m_fHoverSize = 0;
float SelectionCircle::m_fTargetSize = 0;

float SelectionCircle::m_fSelectionOffset = 0;
float SelectionCircle::m_fHoverOffset = 0;
float SelectionCircle::m_fTargetOffset = 0;

//-----------------------------------------------------------------------------
// Clean up any OGRE-related variables here
//-----------------------------------------------------------------------------
void SelectionCircle::Destroy()
{
    HideHoverCircle();
    HideSelectionCircle();

    delete m_pSelectionFrustum;
    delete m_pHoverFrustum;

    // MaterialPtr didn't work as a static variable, but we can still cast down
    // and remove the passes without any problem
    static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName(LevelParser::GetMaterialName()))->
        getTechnique(0)->removePass(static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName(LevelParser::GetMaterialName()))->
        getTechnique(0)->getPass("SelectionCircle")->getIndex());
    static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName(LevelParser::GetMaterialName()))->
        getTechnique(0)->removePass(static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName(LevelParser::GetMaterialName()))->
        getTechnique(0)->getPass("HoverCircle")->getIndex());
}
//-----------------------------------------------------------------------------
// Set up the frustums, nodes and add the material passes required for the
// projected textures here for each of our circles
//-----------------------------------------------------------------------------
void SelectionCircle::Initialize(std::string p_sTextureName)
{
    // Set up the selection circle
    m_pSelectionFrustum = new Ogre::Frustum();
    m_pSelectionFrustum->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    m_pSelectionNode = Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->getRootSceneNode()->createChildSceneNode("SelectionNode");
    m_pSelectionNode->attachObject(m_pSelectionFrustum);
    m_pSelectionNode->setPosition(-100,0,-100);
    Ogre::Pass* pass = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName(LevelParser::GetMaterialName()))->getTechnique(0)->createPass();
    pass->setName("SelectionCircle");
    pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    pass->setDepthBias(1);
    pass->setLightingEnabled(false);

    Ogre::TextureUnitState *texState = pass->createTextureUnitState("Decal.png");
    texState->setProjectiveTexturing(true, m_pSelectionFrustum);
    texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    texState->setTextureFiltering(Ogre::FO_POINT, Ogre::FO_LINEAR, Ogre::FO_NONE);

    // Set up the hover-over circle
    m_pHoverFrustum = new Ogre::Frustum();
    m_pHoverFrustum->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    m_pHoverNode = Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->getRootSceneNode()->createChildSceneNode("HoverNode");
    m_pHoverNode->attachObject(m_pHoverFrustum);
    m_pHoverNode->setPosition(-100,0,-100);
    pass = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName(LevelParser::GetMaterialName()))->getTechnique(0)->createPass();
    pass->setName("HoverCircle");
    pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    pass->setDepthBias(1);
    pass->setLightingEnabled(false);

    texState = pass->createTextureUnitState("DecalTransparent.png");
    texState->setProjectiveTexturing(true, m_pHoverFrustum);
    texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    texState->setTextureFiltering(Ogre::FO_POINT, Ogre::FO_LINEAR, Ogre::FO_NONE);

    // Set up the priority target circle
    m_pTargetFrustum = new Ogre::Frustum();
    m_pTargetFrustum->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    m_pTargetNode = Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->getRootSceneNode()->createChildSceneNode("TargetNode");
    m_pTargetNode->attachObject(m_pTargetFrustum);
    m_pTargetNode->setPosition(-100,0,-100);
    pass = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName(LevelParser::GetMaterialName()))->getTechnique(0)->createPass();
    pass->setName("TargetCircle");
    pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    pass->setDepthBias(1);
    pass->setLightingEnabled(false);

    texState = pass->createTextureUnitState("DecalTarget.png");
    texState->setProjectiveTexturing(true, m_pTargetFrustum);
    texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    texState->setTextureFiltering(Ogre::FO_POINT, Ogre::FO_LINEAR, Ogre::FO_NONE);

    // Orient the frustums down the y and tell them the aspect ratio of the screen
    m_pSelectionNode->setOrientation(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3::UNIT_X));
    m_pSelectionFrustum->setAspectRatio(1);
    m_pSelectionFrustum->setOrthoWindowHeight(1);
    m_pSelectionFrustum->setQueryFlags(1<<6);

    m_pHoverNode->setOrientation(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3::UNIT_X));
    m_pHoverFrustum->setAspectRatio(1);
    m_pHoverFrustum->setOrthoWindowHeight(1);

    m_pTargetNode->setOrientation(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3::UNIT_X));
    m_pTargetFrustum->setAspectRatio(1);
    m_pTargetFrustum->setOrthoWindowHeight(1);
    m_pHoverFrustum->setQueryFlags(1<<6);
}

//-----------------------------------------------------------------------------
// Lock the selection circle to the position of a given scene node
//-----------------------------------------------------------------------------
void SelectionCircle::LockTo(Ogre::SceneNode* p_pSceneNode, float p_fSize, float p_fOffset)
{
    // First we clear the hovering circle over this object and the previous selection circle
    HideSelectionCircle();
    HideHoverCircle();

    m_pLockNode = p_pSceneNode;
    m_fSelectionSize = p_fSize;
    m_fSelectionOffset = p_fOffset;
}
//-----------------------------------------------------------------------------
// Lock the hovering circle to the position of the given scene node
//-----------------------------------------------------------------------------
void SelectionCircle::HoverOver(Ogre::SceneNode* p_pSceneNode, float p_fSize, float p_fOffset)
{
    // Make sure the previous target hovered over is always hidden first
    HideHoverCircle();
    
    // If we are hovering over the selected or targeted object, we don't need to show the hover circle
    if ((m_pLockNode != p_pSceneNode) && (m_pTargetNode != p_pSceneNode))
    {
        m_pHoverLockNode = p_pSceneNode;
        m_fHoverSize = p_fSize;
        m_fHoverOffset = p_fOffset;
    }
}
//-----------------------------------------------------------------------------
// Lock the target circle to the position of the given scene node
//-----------------------------------------------------------------------------
void SelectionCircle::LockTarget(Ogre::SceneNode* p_pSceneNode, float p_fSize, float p_fOffset)
{
    // Hide the hover circle if it was over the target
    if (m_pHoverNode == p_pSceneNode)
        HideHoverCircle();
    
    m_pTargetLockNode = p_pSceneNode;
    m_fTargetSize = p_fSize*1.5;
    m_fTargetOffset = p_fOffset;
}
//-----------------------------------------------------------------------------
// Remove any circles locked to the given scene node.  Called when an object
// is deleted so the circles don't try to update on a missing node
//-----------------------------------------------------------------------------
void SelectionCircle::Deselect(Ogre::SceneNode* p_pSceneNode)
{
    // Check if the given object was selected, and if it was deselect it
    if (m_pLockNode == p_pSceneNode)
        HideSelectionCircle();

    // Even if the object was not selected, it may have the hovering circle on it, so we check for that as well
    if (m_pHoverLockNode == p_pSceneNode)
        HideHoverCircle();

    // Same for the target circle
    if (m_pTargetLockNode == p_pSceneNode)
        HideTargetCircle();
}
//-----------------------------------------------------------------------------
// Hide the selection circle by moving it off the map.  
//
// For this and the following hide methods I previously removed the extra
// circle pass from the material each time it was hidden.  This broke following
// the addition of the shadows, so it does does this slight hack instead.
//-----------------------------------------------------------------------------
void SelectionCircle::HideSelectionCircle()
{
    if (m_pLockNode)
    {
        m_pLockNode = 0;
        m_pSelectionNode->setPosition(-300,0,-300);
    }
}
//-----------------------------------------------------------------------------
// Hide the hovering circle by moving it off the map
//-----------------------------------------------------------------------------
void SelectionCircle::HideHoverCircle()
{
    if (m_pHoverLockNode)
    {
       m_pHoverLockNode = 0;
       m_pHoverNode->setPosition(-300,0,-300);
    }
}
//-----------------------------------------------------------------------------
// Hide the target circle by moving it off the map
//-----------------------------------------------------------------------------
void SelectionCircle::HideTargetCircle()
{
    if (m_pTargetLockNode)
    {
       m_pTargetLockNode = 0;
       m_pTargetNode->setPosition(-300,0,-300);
    }
}
//-----------------------------------------------------------------------------
// Update the position of each of the circles if they are on the field,
// following the position of the scene node they are locked to
//-----------------------------------------------------------------------------
void SelectionCircle::Update()
{
    if (m_pLockNode)
    {
        // Use the x and z of the node, and get the y of the terrain so the scaling
        // value is always consistent between objects
        m_pSelectionNode->setPosition(m_pLockNode->getPosition().x + m_fSelectionOffset, 
                                      Terrain::GetHeight(m_pLockNode->getPosition().x, 
                                                         m_pLockNode->getPosition().z) + m_fSelectionSize + 1000, 
                                      m_pLockNode->getPosition().z);

        // Set the size of the circle
        m_pSelectionFrustum->setOrthoWindowHeight(m_fSelectionSize);
    }
    if (m_pHoverLockNode)
    {
        // Use the x and z of the node, and get the y of the terrain so the scaling
        // value is always consistent between objects
        m_pHoverNode->setPosition(m_pHoverLockNode->getPosition().x + m_fHoverOffset, 
                                  Terrain::GetHeight(m_pHoverNode->getPosition().x, 
                                                     m_pHoverNode->getPosition().z) + m_fHoverSize + 1000,  
                                  m_pHoverLockNode->getPosition().z);

        // Set the size of the circle
        m_pHoverFrustum->setOrthoWindowHeight(m_fHoverSize);
    }
    if (m_pTargetLockNode)
    {
        // Use the x and z of the node, and get the y of the terrain so the scaling
        // value is always consistent between objects
        m_pTargetNode->setPosition(m_pTargetLockNode->getPosition().x + m_fTargetOffset, 
                                  Terrain::GetHeight(m_pTargetNode->getPosition().x, 
                                                     m_pTargetNode->getPosition().z) + m_fTargetSize + 1000,  
                                  m_pTargetLockNode->getPosition().z);

        // Set the size of the circle
        m_pTargetFrustum->setOrthoWindowHeight(m_fTargetSize);
    }
}