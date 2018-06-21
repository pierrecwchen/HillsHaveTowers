//-----------------------------------------------------------------------------
// BaseEnemy.h                                                  By: Keenan Johnston
// 
// 
//-----------------------------------------------------------------------------

#ifndef ENEMY_H
#define ENEMY_H

#include <iostream>
#include <sstream>

#include "EnemyManager.h"
#include "Recipient.h"

class BaseEnemy : public Recipient, public Selectable
{
	public:
        // Reference counting methods (need to be fixed/changed)
        inline void IncrementReference() { m_iRefCount++; }
        void DecrementReference();
		
		void Attacked(float power, bool pierce);
        bool InRange(Ogre::Vector3 p_v3Loc, float p_fRange);
		void GetShortestPath();
		
        inline State GetState() { return m_eState; }
		void Update(const Ogre::FrameEvent& p_feEvent);
		void Render();  // ?
		void HandleEvent();
		
		inline Ogre::Vector3 GetLocation() { return m_snNode->getPosition(); }
	
	protected:
        // Reference count
        int m_iRefCount;
  
		// Enemy related variables
		int m_iHealth, m_iVillagersCarried;
		float m_fDmgReduction, m_fWalkSpeed;
        State m_eState;
        float m_fDeathTimeLeft;
		
		// Ogre related variables
		Ogre::SceneNode* m_snNode;
		Ogre::Entity* m_enEntity;
		Ogre::AnimationState* m_asAnimationState;
	
	private:
    	// Private Constructor (need some kind of identifier for enemy type?)
		Enemy(int p_iHp, int p_iCarry, float p_fArmour, float p_fSpeed,
              string p_sName, string p_sMesh, Ogre::SceneManager* p_smManager);
        ~Enemy();
		
};

#endif