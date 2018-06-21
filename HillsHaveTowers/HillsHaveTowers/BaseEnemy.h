//-----------------------------------------------------------------------------
// BaseEnemy.h                                              By: Keenan Johnston
// 
// Representation of an enemy within the game.  Has various attributes such as
// speed, health, armour, etc.  This class is used in conjunction with the
// Enemy Manager which handles the creation of derived enemy objects.
//
// Base code written by Keenan, pathfinding by Nick, anything else as noted.
//-----------------------------------------------------------------------------

#ifndef BASEENEMY_H
#define BASEENEMY_H

#include <iostream>

#include "Recipient.h"
#include "Selectable.h"
#include "Model.h"
#include "VillagerManager.h"
#include "EventManager.h"
#include "MainCurrency.h"
#include "Currency.h"
#include "SelectionCircle.h"

enum EnemyStates {ES_IDLE, ES_ATTACK, WalkState, DeadState, EscapeState};
enum EnemyAnimations {EA_WALK, EA_DEATH};
enum AttackTypes {AT_STANDARD, AT_FIRE, AT_SNIPER};  // Attacks received from towers

class BaseEnemy : public Selectable, public Hills::Recipient
{
	public:
        friend class EnemyManager;

        // Damage and heal methods that alter the enemy's current health
		void Attacked(float power, bool pierce, AttackTypes p_eAttackType);
        void Healed(int p_iHealth);
        
        // Pathfinding methods (by Nick)
		void SetShortestPath(std::list<Tile> p_lPath);
        inline std::list<Tile> GetShortestPath() { return m_lOriginalPath; }

        // Checks for a few flags
        inline bool CanDelete() { return m_bCanDelete; }
        inline bool HasArmor() { return m_bHasArmor; }
        virtual bool IsIdentified() { return m_bIdentified; }

        inline void SetIdentified(bool p_bIdentified) { m_bIdentified = p_bIdentified; }

        // Check if enemy is within range of a given circle
        bool InRange(Ogre::Vector3 p_v3Loc, float p_fRange, float p_fBlindness = 1.0f);
        
        // Test method
        inline void SetHP(int p_iHp) { m_iCurrentHealth = p_iHp; m_eKillingAttack = AT_STANDARD; }
        inline void SetState(EnemyStates p_eState) { m_eState = p_eState; }
		
        // Getters for some useful values
        inline Ogre::SceneNode* GetSceneNode() { return m_pModel->GetSceneNode(); }
		inline Ogre::Vector3 GetLocation() { return m_pModel->GetLocation(); }
        inline int GetHealth() { return m_iCurrentHealth; }
        inline EnemyStates GetState() { return m_eState; }
        inline Model* GetModel() { return m_pModel; }

        // Handle selection by selection circle with a size parameter
        void Select();

        // Clean up all OGRE-related data here
        void DestroyModel();

        // Update the enemy's state each frame
        virtual void Update(const Ogre::FrameEvent& p_feEvent);

        //inline void Scale(Ogre::Vector3 p_v3Scalar) { m_pModel->Scale(p_v3Scalar); }
        //inline void Translate(Ogre::Vector3 p_v3Translation) { m_pModel->Translate(p_v3Translation); }
        //inline void Rotate(int p_iRotation) { m_pModel->Rotate(p_iRotation); }

		/***********************************************************************************
		*	Added by Pierre
		************************************************************************************/
		virtual const Ogre::AxisAlignedBox& GetBoundingBox() { return m_pBoundingBox->_getWorldAABB(); }
		virtual void ShowObject(bool p_bIsShow);

		// Handle event method, each subclass has their own way to take care of an event.
		virtual void HandleEvent(const Hills::GameEvent& p_geGameEvent);
		/***********************************************************************************/
	
	protected:
		// Enemy attributes
		int m_iCurrentHealth, m_iMaxHealth;
        int m_iVillagersCarried, m_iCurrentVillagersCarried;
		float m_fDmgReduction, m_fWalkSpeed;      
        int m_iHeight;
        Currency m_cLoot;

        // Selection-circle variables
        float m_fCircleSize, m_fCircleOffset;

        // Slow effect variables
        bool m_bSlowed;
        float m_fSlowTime, m_fSpeedModifier;

        bool m_bIdentified;

        //NICK~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
        //floats for the offset relative to each Tile so that enemies don't all walk on the same spot
        float m_fXOffset, m_fZOffset;

        //Helpers for getting the proper direction to walk in
        float GetAngleDifference(float p_fCurrent, float p_fDestination);
        float GetAngleToNextTile();
        
        // Helper method to determine if enemy is on new path
        bool IsOnNewPath();

        //lists that hold the different iterations of the path that the enemy walks.
        //CurrentPath: A list of tiles that the enemy walks, popping off tiles as he walks over them
        //ExitPath: Same as current path but in reverse for when the enemy is leaving
        //OriginalPath: Same as current path except it never has any elements popped off
        //ReceivedPath: The new shortest path if the enemy isn't actually walking on it yet.
        std::list<Tile> m_lOriginalPath;
        std::list<Tile> m_lCurrentPath;
        std::list<Tile> m_lExitPath;
        std::list<Tile> m_lReceivedPath;

        //Where the enemy is currently heading.
        Tile m_tTargetTile;

        bool m_bIsOnCorrectPath;
        bool m_bIsLeaving;

        float m_fTimeSinceEscape;

        bool m_bHasArmor;
        float m_fScreamPitch;

        void PlayDeathSound();
        void PlayCaptureSound();
        virtual void PlayEscapeSound() = 0;
        //~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*

		// Ogre related variables
        Ogre::SceneNode* m_pBoundingBox;
        Model* m_pModel;
        float m_fAnimationSpeed;

        // Healing effects
        Ogre::ParticleSystem* m_pHealParticle;
        float m_fHealEmitter;
        bool m_bHealed;

        // Current state of the enemy
        EnemyStates m_eState;

        // Death-related variables
        AttackTypes m_eKillingAttack;
        float m_fDeathTimeLeft, m_fParticleDelay;
        bool m_bCanDelete, m_bParticleStarted;
        Ogre::ParticleSystem* m_pParticleSystem;
        Ogre::SceneNode* m_pEffectNode;

        // Helper methods for unique death animations from different types of towers
        virtual void DieByFire();
        virtual void DieBySniper();

        // Protected constructor so class can not be initialized normally
        BaseEnemy();
        BaseEnemy(int p_iHp, int p_iCarry, float p_fArmour, float p_fSpeed,
                  std::string p_sMesh, Ogre::SceneManager* p_smManager,
                  std::list<Tile> p_lShortestPath);
        ~BaseEnemy();       
};

#endif