//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
//Model.cpp
//
//PURPOSE:
//Loads a specified model and allows multiple actions to be performed upon it simultaneously
//
//All code here is written by Nick unless otherwise specified
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*

#include "Model.h"
#include "Terrain.h"

#define TO_RADIANS(degrees) degrees * 2 * 3.1415926535 / 360

Model::Model(std::string p_sMesh, std::string p_sName)
{
    //An entity for that mesh is created and attached to a node
    if(p_sName == "NO_NAME")
    {
        m_pEntity = Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->createEntity(p_sMesh);
    }
    else
    {
        m_pEntity = Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->createEntity(p_sName, p_sMesh);
    }
    
    m_pEntity->setCastShadows(true);
    
    //KEENAN --------------------
    if (m_pEntity->hasSkeleton()) // set the animation mode if the mesh has a skeleton
        m_pEntity->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);
    //---------------------------

    m_pSceneNode = Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->getRootSceneNode()->createChildSceneNode();

    m_pSceneNode->attachObject(m_pEntity);

    //All variables are initialized
    m_fTimeSpentTranslating = 0;
    m_fTimeSpentRotating = 0;
    m_fTimeSpentScaling = 0;

    m_bNeedToTranslate = false;
    m_bNeedToRotate = false;
    m_bNeedToScale = false;

    m_v3Position = Ogre::Vector3::ZERO;
    m_v3Scale = Ogre::Vector3(1, 1, 1);
    m_v3InitialScale = Ogre::Vector3(1, 1, 1);
    m_fRotation = 0;

    m_v3TranslationDif = Ogre::Vector3::ZERO;
    m_v3ScaleDif = Ogre::Vector3::ZERO;
    m_fRotationDif = 0;
    m_eAxis = RA_Y;  // default to y-axis

    //KEENAN-----------------------------------
    m_bIsAnimated = false;    // assume model is not animated until one is played
    m_bBlending = false;
    m_fAnimationSpeed = 1.0; // default speed
    m_pBlend = new AnimationBlender(m_pEntity);

    m_fCurrentDrop = 0;
    m_bUseGravity = false;
    m_bUseTerrainHeight = false;
    m_iHeight = 0;

    m_pEntity->setQueryFlags(QF_NULL);
}

Model::~Model()
{
    //!!!!!!
    //I'm not sure what to stick here because it seems Ogre handles the deletion of many
    // objects on its own, even the pointers that are created in here. When I deleted
    // the scene node in this method, it started throwing access violations at the 
    // delete root node method in the Ogre classes.

    m_pEntity = 0;
    m_pSceneNode = 0;
    m_pAnimationState = 0;
	m_pBlend = 0;
}

Model::Model(const Model& p_mOther)
{
    
}

Model Model::operator=(const Model& p_mOther)
{
    return *this;
}

//KEENAN---------------------------------------------
void Model::CleanUp()
{
    // Destroy all attached objects for this scene node
    Ogre::SceneNode::ObjectIterator objectIterator = m_pSceneNode->getAttachedObjectIterator();

    while (objectIterator.hasMoreElements())
        m_pSceneNode->getCreator()->destroyMovableObject(objectIterator.getNext());
    
    m_pSceneNode->removeAndDestroyAllChildren();
    m_pSceneNode->getParentSceneNode()->removeAndDestroyChild(m_pSceneNode->getName());
}

void Model::AddEntity(std::string p_sName)
{
    m_pSceneNode->attachObject(Ogre::Root::getSingletonPtr()->getSceneManager(BaseApplication::GetSceneManagerName())->createEntity(p_sName));
}
//---------------------------------------------------

//This method handles all progression in all transformations and animations per frame
void Model::Update(const Ogre::FrameEvent &p_feEvent)
{
    //If we need to translate the model
    if(m_bNeedToTranslate)
    {
        //If we still have time remaining before we finish our translation period
        
        //We translate the node relative to how long the last frame was divided by how many seconds it's
        // supposed to take to do this. We then append the amount of time to our time spent translating
        
        //KEENAN-----------------------------------------------------------------------------
        if (m_bUseGravity)
        {
            m_fCurrentDrop -= p_feEvent.timeSinceLastFrame * 2 * BaseApplication::GetGameSpeed();  // arbritary value for gravity
            m_pSceneNode->translate(m_v3TranslationDif.x * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() / m_fTranslationPeriod,
                                    m_fCurrentDrop,
                                    m_v3TranslationDif.z * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() / m_fTranslationPeriod);
            // Perform a check to see if the bullet is near 0 on the Y and stop translating if it is 
            // (so it doesn't go below the world)
            if (m_pSceneNode->getPosition().y < Terrain::GetHeight(m_pSceneNode->getPosition().x, m_pSceneNode->getPosition().z))
                StopMovement();
        }
        // If we want to use the height of the terrain, we get the height at the point we are translating to and
        // subtract the current height from it to get the proper one
        else if (m_bUseTerrainHeight)
        {
            float height = Terrain::GetHeight(m_pSceneNode->getPosition().x + (m_v3TranslationDif.x * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() / m_fTranslationPeriod), 
                                              m_pSceneNode->getPosition().z + (m_v3TranslationDif.z * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() / m_fTranslationPeriod));
            m_pSceneNode->translate(Ogre::Vector3(m_v3TranslationDif.x * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() / m_fTranslationPeriod,
                                                  height - m_pSceneNode->getPosition().y + m_iHeight,
                                                  m_v3TranslationDif.z * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() / m_fTranslationPeriod));
        }
        // Otherwise, translate without taking anything else into consideration
        else
            m_pSceneNode->translate(m_v3TranslationDif * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() / m_fTranslationPeriod);

        m_fTimeSpentTranslating += p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();
        m_v3Position = m_pSceneNode->getPosition();
        
        //If we are over the time remaining (if 4.99 seconds remains out of 5, we still need to translate
        // no matter how long the next frame is)
        if(m_fTimeSpentTranslating > m_fTranslationPeriod)
        {
            //We flag ourselves as finished translating, set the time we spent translating to 0 since we're done
            // and set the node's position to the EXACT destination.
            m_bNeedToTranslate = false;
            m_fTimeSpentTranslating = 0;
            //m_pSceneNode->setPosition(m_v3Position);
        }
    }

    if(m_bNeedToRotate)
    {
        if(m_fTimeSpentRotating < m_fRotationPeriod)
        {
            //KEENAN------------------------------------------------------------------------------
            if (m_eAxis == RA_X)
                m_pSceneNode->roll(Ogre::Degree(m_fRotationDif * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() / m_fRotationPeriod));
            else if (m_eAxis == RA_Y)
                m_pSceneNode->yaw(Ogre::Degree(m_fRotationDif * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() / m_fRotationPeriod));
            else if (m_eAxis == RA_Z)
                m_pSceneNode->pitch(Ogre::Degree(m_fRotationDif * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() / m_fRotationPeriod));
            //------------------------------------------------------------------------------------

            m_fTimeSpentRotating += p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();
        }
        else
        {
            m_bNeedToRotate = false;
            m_fTimeSpentRotating = 0;
            //Since there is no setYaw method, we need to specify the EXACT final rotation with respect to our original
            // rotation.
            m_pSceneNode->yaw(Ogre::Degree(m_fRotationDif * (m_fRotationPeriod - m_fTimeSpentRotating) / m_fRotationPeriod));
        }
    }

    if(m_bNeedToScale)
    {
        if(m_fTimeSpentScaling < m_fScalePeriod)
        {
            //Since scale is multiplicative and not additive, and we want our node to scale linearly, the formula required
            // what our original scale was to work.
            m_v3Scale += (m_v3ScaleDif * m_v3InitialScale - m_v3InitialScale) * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() / m_fScalePeriod;
            m_pSceneNode->setScale(m_v3Scale);
            m_fTimeSpentScaling += p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();
        }
        else
        {
            m_bNeedToScale = false;
            m_fTimeSpentScaling = 0;
            m_v3Scale = m_v3InitialScale * m_v3ScaleDif;
            m_pSceneNode->setScale(m_v3Scale);
        }
    }

    //KEENAN-------------------------------------------------
    // Update animation of the model if applicable
    if (m_bIsAnimated)
        m_pBlend->addTime(p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed()*m_fAnimationSpeed);
    //-------------------------------------------------------
}

//The following three methods transform the model instantly with no transition
void Model::Translate(Ogre::Vector3 p_v3Translation)
{
    m_v3Position += p_v3Translation;
    
    // Check if the translate vector goes off the edge of the map
    if (m_v3Position.x < 2) m_v3Position.x = 2;
    if (m_v3Position.z < 2) m_v3Position.z = 2;
    if (m_v3Position.x > Terrain::GetWidth()-2) m_v3Position.x = Terrain::GetWidth()-2;
    if (m_v3Position.z > Terrain::GetLength()-2) m_v3Position.z = Terrain::GetLength()-2;

    m_pSceneNode->translate(m_v3Position - m_pSceneNode->getPosition());
}

//Rotates the model relative to its current orientation
void Model::Rotate(float p_fDegree)
{
    //KEENAN-------------------------------------------------
    if (m_eAxis == RA_X)
        m_pSceneNode->roll(Ogre::Degree(p_fDegree));
    else if (m_eAxis == RA_Y)
        m_pSceneNode->yaw(Ogre::Degree(p_fDegree));
    else if (m_eAxis == RA_Z)
        m_pSceneNode->pitch(Ogre::Degree(p_fDegree));
    //-------------------------------------------------------

    m_fRotation += p_fDegree;

    if (m_fRotation >= 360)
        m_fRotation -= 360;
    else if (m_fRotation < 0)
        m_fRotation += 360;
}

//Scales the model relative to its current scale.
void Model::Scale(Ogre::Vector3 p_v3Scalar)
{
    m_pSceneNode->scale(p_v3Scalar);
    m_v3Scale *= p_v3Scalar;
    m_v3InitialScale = m_v3Scale;
}

//Takes the direction that the model is currently facing and moves them forward in that direction.
void Model::MoveForward(float p_fDistance)
{
    //Using trigonometry, we get the distance to move in the X and Z and move them forward that much
    float xDif = p_fDistance * sin(TO_RADIANS(m_fRotation));
    float zDif = p_fDistance * cos(TO_RADIANS(m_fRotation));

    //And we translate him that distance
    Translate(Ogre::Vector3(xDif, 0, zDif));
}

//Similar to the last method, we move the model forward given its current rotation, however this method moves them
//relative to the terrain height. It also ensures that they maintain an even speed when moving up and down hills.
void Model::MoveForwardOverTerrain(float p_fDistance, float p_fFrameTime, float p_fYOffset)
{
    if(!m_bNeedToTranslate)
    {
        float actualDistance = p_fDistance * p_fFrameTime;

        //Just like before, we find the distance we need to move in the X and Z
        float xDif = actualDistance * sin(TO_RADIANS(m_fRotation));
        float zDif = actualDistance * cos(TO_RADIANS(m_fRotation));

        //We compare those differences to our current position to get our next X and Z coordinates.
        float xLoc = xDif + m_v3Position.x;
        float zLoc = zDif + m_v3Position.z;

        //Using that, we find out where that would put us by finding the height at the new X and Z coordinates.
        Ogre::Vector3 originalNext(xLoc, Terrain::GetHeight(xLoc, zLoc) + p_fYOffset, zLoc);
        //Ogre::Vector3 tempNext(xLoc, p_fYOffset, zLoc);

        Ogre::Vector3 tempNext = originalNext;

        int tries = 0;

        //If the distance between the two points is too different from the actual distance we want to go.
        while((tempNext.distance(m_v3Position) > actualDistance * 1.1 || tempNext.distance(m_v3Position) < actualDistance / 1.1)
              && tries < 8)
        {
            //We find how much bigger or smaller the new distance then our desired distance
            float distanceScaler = tempNext.distance(m_v3Position) / actualDistance;

            //We divide it off the distances we need to move in the X and Z
            xDif /= distanceScaler;
            zDif /= distanceScaler;

            //We use those differences to find our new X and Z coordinates
            xLoc = xDif + m_v3Position.x;
            zLoc = zDif + m_v3Position.z;

            //and then we find our new potential location again
            tempNext = Ogre::Vector3(xLoc, Terrain::GetHeight(xLoc, zLoc) + p_fYOffset, zLoc);
            //Ogre::Vector3 tempNext(xLoc, p_fYOffset, zLoc);

            tries++;
        }
        
        //The player model has a slight offset in the Y that needs to be applied to him, so we just add it here at the very end
        tempNext.y += p_fYOffset;

        if(tries == 8)
        {
            TranslateToWithSpeed(p_fDistance, originalNext);
        }
        else
        {
            //We then translate him directly to that specific coordinate.
            TranslateTo(tempNext);
        }
    }
}


//The following three methods transform the model relative to the origin
void Model::TranslateTo(Ogre::Vector3 p_v3Translation)
{
    Translate(p_v3Translation - m_v3Position);
}

void Model::RotateTo(float p_fDegree)
{
    Rotate(p_fDegree - m_fRotation);
}

void Model::ScaleTo(Ogre::Vector3 p_v3Scalar)
{
    Scale(p_v3Scalar / m_v3Scale);
}

//The following three methods set up the update method for a transitioned transformation by
// setting the final transformation coordinate/rotation/scale relative to their current
// position, the period, and specifying that we need to transform.
void Model::TranslateOverTime(float p_fPeriod, Ogre::Vector3 p_v3Translation)
{
    if (p_fPeriod == 0)
        return;

    // Check if the translate vector goes off the edge of the map
    Ogre::Vector3 temp = p_v3Translation + m_v3Position;

    if (temp.x < 2) temp.x = 2;
    if (temp.z < 2) temp.z = 2;
    if (temp.x > Terrain::GetWidth()-2) temp.x = Terrain::GetWidth()-2;
    if (temp.z > Terrain::GetLength()-2) temp.z = Terrain::GetLength()-2;
    
    m_fTranslationPeriod = p_fPeriod;
    m_v3TranslationDif = temp - m_v3Position;
    m_bNeedToTranslate = true;
}

void Model::RotateOverTime(float p_fPeriod, float p_fDegree)
{
    m_fRotationPeriod = p_fPeriod;
    m_fRotationDif = p_fDegree;
    m_fRotation += p_fDegree;
    m_bNeedToRotate = true;
}

void Model::ScaleOverTime(float p_fPeriod, Ogre::Vector3 p_v3Scalar)
{
    m_fScalePeriod = p_fPeriod;
    m_v3ScaleDif = p_v3Scalar;
    m_v3InitialScale = m_v3Scale;
    m_bNeedToScale = true;
}

//The following three methods are the same as the above methods except they are relative
// to the origin instead of their current transformation
void Model::TranslateToOverTime(float p_fPeriod, Ogre::Vector3 p_v3Translation)
{
    TranslateOverTime(p_fPeriod, p_v3Translation - m_v3Position);
}

void Model::ScaleToOverTime(float p_fPeriod, Ogre::Vector3 p_v3Scalar)
{
    ScaleOverTime(p_fPeriod, p_v3Scalar / m_v3Scale);
}

void Model::RotateToOverTime(float p_fPeriod, float p_fRotation)
{
    RotateOverTime(p_fPeriod, p_fRotation - m_fRotation);
}

void Model::TranslateToWithSpeed(float p_fSpeed, Ogre::Vector3 p_v3Destination)
{
    float distance = p_v3Destination.distance(m_v3Position);
    float period = distance / p_fSpeed;

    TranslateToOverTime(period, p_v3Destination);
}
//-----------------------------------------------------------------------------
// Added by Keenan
//
// This method translates an object using an arbritrarily defined value for
// gravity which compounds each frame
//-----------------------------------------------------------------------------
void Model::TranslateToWithGravity(float p_fSpeed, Ogre::Vector3 p_v3Destination)
{
    float distance = p_v3Destination.distance(m_v3Position);
    float period = distance / p_fSpeed;
    m_bUseGravity = true;

    TranslateToOverTime(period, p_v3Destination);
}

float Model::GetAngleTo(Ogre::Vector3 p_vLoc)
{
    float rotation = atan((p_vLoc.x - m_pSceneNode->getPosition().x)/(p_vLoc.z - m_pSceneNode->getPosition().z));
    rotation = rotation * 180 / 3.14159;
	if ((p_vLoc.z - m_pSceneNode->getPosition().z) < 0)
		return rotation+180;
	else
		return rotation;
}

//-----------------------------------------------------------------------------
// Added by Keenan and Pierre
//
// This method rotates the model in the y in the direction of a given vector.  
// Used to move the rotating tower tops
//-----------------------------------------------------------------------------
void Model::FaceTo(Ogre::Vector3 p_v3Loc)
{
    float rotation = atan((p_v3Loc.x - m_pSceneNode->getPosition().x)/(p_v3Loc.z - m_pSceneNode->getPosition().z));
    rotation = rotation * 180 / 3.14159;
	if ((p_v3Loc.z - m_pSceneNode->getPosition().z) < 0)
		RotateTo(rotation+180);
	else
		RotateTo(rotation);
}
//-----------------------------------------------------------------------------
// Added by Keenan and Pierre
//
// Same as the above method, except we do a negative rotation.  Needed for a
// few models which wouldn't export properly from Blender
//-----------------------------------------------------------------------------
void Model::FaceToReverse(Ogre::Vector3 p_v3Loc)
{
    float rotation = atan((p_v3Loc.x - m_pSceneNode->getPosition().x)/(p_v3Loc.z - m_pSceneNode->getPosition().z));
    rotation = rotation * 180 / 3.14159;
	if ((p_v3Loc.z - m_pSceneNode->getPosition().z) < 0)
		RotateTo(-rotation+180);
	else
		RotateTo(-rotation);
}
//-----------------------------------------------------------------------------
// Added by Keenan
//
// This method uses a quarternion to rotate on every axis to face a given
// vector.  Used to orient projectiles like arrows at their targets.
//-----------------------------------------------------------------------------
void Model::FaceToByQuaternion(Ogre::Vector3 p_v3Loc)
{
    Ogre::Vector3 m_v3Source = m_pSceneNode->getOrientation() * Ogre::Vector3::UNIT_Z;
    Ogre::Vector3 direction = (p_v3Loc - m_pSceneNode->getPosition());
	direction.normalise();
    Ogre::Quaternion quat = m_v3Source.getRotationTo(direction);
    m_pSceneNode->rotate(quat);
}
//-----------------------------------------------------------------------------
// Added by Keenan
//
// This method should change the model's colour by cloning its material,
// changing the colour values and applying it, but it does not seem to work
// currently (shadows seem to be the problem)
//-----------------------------------------------------------------------------
void Model::SetColour(Ogre::ColourValue m_cColour)
{
    // First we need to make a copy of the model's current material, otherwise we will affect all
    // models using this material
    //m_pEntity->getSubEntity(0)->getMaterial()->getTechnique(0)->getPass(1)->setDiffuse(Ogre::ColourValue(0.4,0.4,1));
    //m_pEntity->getSubEntity(0)->getMaterial()->getTechnique(0)->getPass(1)->setSpecular(Ogre::ColourValue(0.4,0.4,1));
    //m_pEntity->getSubEntity(0)->getMaterial()->getTechnique(0)->getPass(1)->setAmbient(Ogre::ColourValue(0.4,0.4,1));
    //Ogre::MaterialPtr currentMaterial = m_pEntity->getSubEntity(0)->getMaterial();
    //m_pMaterial = currentMaterial->clone(m_pEntity->getSubEntity(0)->getMaterial()->getName() + m_pSceneNode->getName(), false, "");
    //m_pMaterial->setAmbient(m_cColour);
    //m_pMaterial->setDiffuse(m_cColour);
    //m_pMaterial->setSpecular(m_cColour);

    /*
    m_pMaterial->getTechnique(0)->getPass(0)->setAmbient(m_cColour);
    m_pMaterial->getTechnique(0)->getPass(0)->setDiffuse(m_cColour);
    m_pMaterial->getTechnique(0)->getPass(0)->setSpecular(m_cColour);
    m_pMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(m_cColour);
    m_pMaterial->getTechnique(0)->getPass(1)->setAmbient(m_cColour);
    m_pMaterial->getTechnique(0)->getPass(1)->setDiffuse(m_cColour);
    m_pMaterial->getTechnique(0)->getPass(1)->setSpecular(m_cColour);
    m_pMaterial->getTechnique(0)->getPass(1)->setSelfIllumination(m_cColour);
    */

    m_pEntity->getSubEntity(0)->setMaterialName(m_pEntity->getSubEntity(0)->getMaterialName() + "_burnt");
}
//-----------------------------------------------------------------------------
// Added by Keenan
//
// Returns the name of the animation currently playing.
//-----------------------------------------------------------------------------
std::string Model::GetAnimationName() 
{ 
    if (!m_bBlending) 
        return m_pAnimationState->getAnimationName(); 
    else 
        return m_pBlend->getTarget()->getAnimationName(); 
}
//-----------------------------------------------------------------------------
// Added by Keenan
//
// Set whether we would like to use animation blending for this model.
//-----------------------------------------------------------------------------
void Model::SetAnimationBlending(bool p_bBlend)
{
    if (p_bBlend)
        m_bBlending = true;
    else
        m_bBlending = false;
}
//-----------------------------------------------------------------------------
// Added by Keenan
//
// Play the given animation.  We use the blender whether the model uses
// blending or not because it supports non-blended animations as well.
//-----------------------------------------------------------------------------
void Model::PlayAnimation(std::string p_sName, bool p_bLoop) 
{     
    if (!m_bBlending)
    {
        /*
        // if there was a previous animation, stop it so no unintended blending occurs
        if (m_sAnimationName != "")
            m_pAnimationState->setEnabled(false);
        
        // Store the name of the animation to be played and get it from our entity
        m_sAnimationName = p_sName;
        m_pAnimationState = m_pEntity->getAnimationState(m_sAnimationName);

        // If the animation we want to play a non-looping animation that has already
        // been played, we need to reset the time of the animation
        if (m_pAnimationState->hasEnded())
            m_pAnimationState->setTimePosition(0);

        // Set other flags for animation
        m_pAnimationState->setLoop(p_bLoop);
        m_pAnimationState->setEnabled(true);
        */
        if (m_pBlend->getFirstAnimation())
            m_pBlend->init(p_sName, p_bLoop);
        else
            m_pBlend->blend(p_sName, AnimationBlender::BlendSwitch, 0.2, p_bLoop);
    }
    else if (m_bBlending)
    {
        if (m_pBlend->getFirstAnimation())
            m_pBlend->init(p_sName, p_bLoop);
        else
            m_pBlend->blend(p_sName, AnimationBlender::BlendWhileAnimating, 0.2, p_bLoop);
    }

    if (!m_bIsAnimated)
        m_bIsAnimated = true;
}
//-----------------------------------------------------------------------------
// Added by Keenan
//
// Add time to the playing animation, on top of the time added each frame
//-----------------------------------------------------------------------------
void Model::AddAnimationTime(Ogre::Real p_rTime)
{
    m_pBlend->addTime(p_rTime);
}