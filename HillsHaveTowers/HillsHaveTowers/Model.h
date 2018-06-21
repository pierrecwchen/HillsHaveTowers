//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
//Model.h
//
//PURPOSE:
//Loads a specified model and allows multiple actions to be performed upon it simultaneously
//
//All code here was written by Nick unless stated otherwise
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*

#ifndef MODEL_H
#define MODEL_H

#include <cmath>
#include <OgreEntity.h>
#include <OgreSceneManager.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h> //Including this fixed an incomplete declaration error I had in the source code, and for some reason
// including this requires I include all OIS stuff as well

#include "BaseApplication.h"
#include "AnimationBlender.h"


//Rotation axes by which axis they rotate about
enum RotationAxis {RA_X, RA_Y, RA_Z};

//The query flags which classify each type of bounding box so that RaySceneQueries only check specific ones
//There is no QF_TILE because tiles are not considered entities, just freroaming bounding boxes.
enum QueryFlag
{
    QF_RESOURCES = 1<<0,
    QF_ENEMIES = 1<<1,
    QF_PLAYER = 1<<2,
    QF_VILLAGERS = 1<<3,
    QF_TOWERS = 1<<4,
    QF_TERRAIN = 1<<5,
    QF_NULL = 1<<6
};

class Model
{
public:
    //Empty Constructor
    Model() {}

    // Constructor
    Model(std::string p_sMesh, std::string p_sName = "NO_NAME");

    //pointer shenanigans
    ~Model();
    Model(const Model&);
    Model operator=(const Model& p_mOther);

    //Sets the query flag so that RaySceneQueries will only check certain models
    inline void SetQueryFlag(QueryFlag p_qfFlag) { m_pEntity->setQueryFlags(p_qfFlag); }

    //All of these instantly perform actions upon cubes
    void Translate(Ogre::Vector3 p_v3Translation);
    void Rotate(float p_fDegree);
    void Scale(Ogre::Vector3 p_v3Scalar);

    //Translates the model forward relative to its current angle
    void MoveForward(float p_fDistance);
    void MoveForwardOverTerrain(float p_fDistance, float p_fFrameTime, float p_fYOffset);

    //Transforms the model to a specific location, orientation or scale relative to the world
    void TranslateTo(Ogre::Vector3 p_v3Translation);
    void ScaleTo(Ogre::Vector3 p_v3Scalar);

    //RotateTo rotates in yaw, pitch or roll, depending on what is currently set
    void RotateTo(float p_fDegree);

    //These will perform each action over time, simultaneously if many are called. The first
    // parameter is the period IN SECONDS it takes to carry out your action, the second parameter
    // is the same as the parameters for the above methods.
    void TranslateOverTime(float p_fPeriod, Ogre::Vector3 p_v3Translation);
    void RotateOverTime(float p_fPeriod, float p_fDegree);
    void ScaleOverTime(float p_fPeriod, Ogre::Vector3 p_v3Scalar);

    //These are similar to above, except they translate and scale respective to the origin rather
    // then the entity's current position
    void TranslateToOverTime(float p_fPeriod, Ogre::Vector3 p_v3Translation);
    void ScaleToOverTime(float p_fPeriod, Ogre::Vector3 p_v3Scalar);
    void RotateToOverTime(float p_fPeriod, float p_fRotation);

    //Similar to TranslateToOverTime except that it accepts a speed in the form of units/s
    void TranslateToWithSpeed(float p_fSpeed, Ogre::Vector3 p_v3Destination);    

    //These getters inform whether or not the entity is currently doing something already
    inline bool IsTranslating() { return m_bNeedToTranslate; }
    inline bool IsRotating() { return m_bNeedToRotate; }
    inline bool IsScaling() { return m_bNeedToScale; }

    inline float TimeLeftTranslating() { return m_fTranslationPeriod - m_fTimeSpentTranslating; }

    // Force stops the entity's movement in cases such as when it dies
    inline void StopMovement() { m_bNeedToTranslate = false; m_fTranslationPeriod = 0; m_v3TranslationDif = 0; m_fTimeSpentTranslating = 0; }

    // Get a pointer to the scene node or entity
    inline Ogre::SceneNode* GetSceneNode() { return m_pSceneNode; }
    inline Ogre::Entity* GetEntity() { return m_pEntity; }

    // Returns location of the model via the scene node
    inline Ogre::Vector3 GetLocation() { return m_pSceneNode->getPosition(); }

    inline float GetRotation() { return m_fRotation; }

    //----------------------------------------------------------------------------------------------------------------------------
    //ADDED BY KEENAN
    //----------------------------------------------------------------------------------------------------------------------------
    // Add another entity to the scene node
    void AddEntity(std::string p_sName);

    // Set the axis to use for rotation
    inline void SetAxis(RotationAxis p_eAxis) { m_eAxis = p_eAxis; }

    // Get the shortest angle to another vector
    float GetAngleTo(Ogre::Vector3 p_vLoc);

    // Various ways to make this model rotate to face a location
    void FaceTo(Ogre::Vector3 p_v3Loc);
    void FaceToReverse(Ogre::Vector3 p_v3Loc);
    void FaceToByQuaternion(Ogre::Vector3 p_v3Loc);

    // Specify if we want translations for this model to conform to the height of the terrain
    inline void UseTerrainHeight(bool p_bUseTerrainHeight) { m_bUseTerrainHeight = p_bUseTerrainHeight; }
    inline void SetHeightAdjustment(int p_iHeight) { m_iHeight = p_iHeight; }

    // Change the vertex colour of the model
    void SetColour(Ogre::ColourValue m_cColour);
  
    // Animation related methods
    void SetAnimationBlending(bool p_bBlend);
    inline Ogre::AnimationState* GetAnimationState() { return m_pBlend->getSource(); }
    void PlayAnimation(std::string p_sName, bool p_bLoop);
    void AddAnimationTime(Ogre::Real p_rTime);
    inline void SetAnimationSpeed(float p_fSpeed) { m_fAnimationSpeed = p_fSpeed; }
    inline bool HasAnimationEnded() { return m_pBlend->getTarget()->hasEnded(); }
    std::string GetAnimationName();

    //Similar to TranslateToOverTime except that it applies gravity to the object
    void TranslateToWithGravity(float p_fPeriod, Ogre::Vector3 p_v3Translation);

    // Dettach and destroy the entity followed by the scene node
    void CleanUp();
    //----------------------------------------------------------------------------------------------------------------------------

    //The magic method that will orchestrate the tweens you perform on the cube. Calling the
    // tween method aboves will not actually move the cube, but rather, set it up for movement.
    // Call this method within the frameRenderingQueued method and hand it the frame event.
    void Update(const Ogre::FrameEvent &p_feEvent);

private:
    //These hold the model's current position, scale and rotation
    float m_fRotation;
    Ogre::Vector3 m_v3Scale;
    Ogre::Vector3 m_v3Position;  

    //These hold the model's goal position, scale and rotation for the XXXOverTime() methods
    Ogre::Vector3 m_v3TranslationDif;
    Ogre::Vector3 m_v3ScaleDif;
    Ogre::Vector3 m_v3InitialScale;
    float m_fRotationDif;

    //bools for the update method which determine whether or not the model is done doing
    // its respective task
    bool m_bNeedToRotate;
    bool m_bNeedToScale;
    bool m_bNeedToTranslate;

    //How long the model needs to do its respective task for in seconds
    float m_fTranslationPeriod;
    float m_fRotationPeriod;
    float m_fScalePeriod;

    //How long the model has been doing its respective task in seconds
    float m_fTimeSpentRotating;
    float m_fTimeSpentScaling;
    float m_fTimeSpentTranslating;

    // Pointer to material, if needed
    Ogre::MaterialPtr m_pMaterial;

    //Ogre stuff for models and animations
    Ogre::SceneNode* m_pSceneNode;
    Ogre::Entity* m_pEntity;

    // ----- Added by Keenan -----

    // Axis to use for rotation (added for models who won't properly export from Blender)
    RotationAxis m_eAxis;

    // Animation variables
    Ogre::AnimationState* m_pAnimationState;
    float m_fAnimationSpeed;
    bool m_bIsAnimated;
    std::string m_sAnimationName;
    AnimationBlender* m_pBlend;
    bool m_bBlending;

    // Variables for using terrain height
    bool m_bUseTerrainHeight;
    int m_iHeight;

    // Experimental values for bullet drop
    float m_fCurrentDrop;
    bool m_bUseGravity;
    //----------------------------
};

#endif

