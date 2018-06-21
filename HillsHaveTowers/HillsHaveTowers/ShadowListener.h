// Credit to nullsquared of the Ogre forums, used from his shadows demo

#ifndef SHADOWLISTENER_H
#define SHADOWLISTENER_H

class ShadowListener: public Ogre::SceneManager::Listener
{
    // this is a callback we'll be using to set up our shadow camera
    void shadowTextureCasterPreViewProj(Ogre::Light *light, Ogre::Camera *cam, size_t)
    {
        // basically, here we do some forceful camera near/far clip attenuation
        // yeah.  simplistic, but it works nicely.  this is the function I was talking
        // about you ignoring above in the Mgr declaration.
        float range = light->getAttenuationRange();
        cam->setNearClipDistance(0.01);
        cam->setFarClipDistance(range);
        // we just use a small near clip so that the light doesn't "miss" anything
        // that can shadow stuff.  and the far clip is equal to the lights' range.
        // (thus, if the light only covers 15 units of objects, it can only
        // shadow 15 units - the rest of it should be attenuated away, and not rendered)
    }

    // these are pure virtual but we don't need them...  so just make them empty
    // otherwise we get "cannot declare of type Mgr due to missing abstract
    // functions" and so on
    void shadowTexturesUpdated(size_t) {}
    void shadowTextureReceiverPreViewProj(Ogre::Light*, Ogre::Frustum*) {}
    void preFindVisibleObjects(Ogre::SceneManager*, Ogre::SceneManager::IlluminationRenderStage, Ogre::Viewport*) {}
    void postFindVisibleObjects(Ogre::SceneManager*, Ogre::SceneManager::IlluminationRenderStage, Ogre::Viewport*) {}
};

#endif