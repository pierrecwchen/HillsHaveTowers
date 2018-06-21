//-----------------------------------------------------------------------------
// AnimationBlender.h                                             By: OGRE Wiki
// 
// Animation blending helper class from the OGRE Wiki, with a few modifications
// made by Keenan.
//-----------------------------------------------------------------------------

class AnimationBlender
 {
 public:
   enum BlendingTransition
   {
      BlendSwitch,         // stop source and start dest
      BlendWhileAnimating,   // cross fade, blend source animation out while blending destination animation in
      BlendThenAnimate      // blend source to first frame of dest, when done, start dest anim
   };
 
 private:
   Ogre::Entity *mEntity;
   Ogre::AnimationState *mSource;
   Ogre::AnimationState *mTarget;
 
   BlendingTransition mTransition;
 
   bool loop;
 
   ~AnimationBlender() {}
 
 public: 
   Ogre::Real mTimeleft, mDuration;
 
   bool complete;
   bool firstAnimation;
 
   void blend( const Ogre::String &animation, BlendingTransition transition, Ogre::Real duration, bool l=true );
   void addTime( Ogre::Real );
   Ogre::Real getProgress() { return mTimeleft/ mDuration; }
   Ogre::AnimationState *getSource() { return mSource; }
   Ogre::AnimationState *getTarget() { return mTarget; }
   inline bool getFirstAnimation() { return firstAnimation; }
   AnimationBlender( Ogre::Entity *);
   void init( const Ogre::String &animation, bool l=true );
 };