//-----------------------------------------------------------------------------
// BaseTower.h                                              By: Keenan Johnston
// 
// Represents a tower within the game.  Holds things such as the stats of the
// the tower, its model and position and the targets it is attacking.
//
// All code by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef BASETOWER_H
#define BASETOWER_H

#include <iostream>

#include "EnemyManager.h"
#include "SoundManager.h"
#include "ProjectileManager.h"
#include "BoundingBox.h"
#include "MainCurrency.h"
#include "Currency.h"

enum TowerStates {TS_BUILD, TS_IDLE, TS_ATTACK, TS_SOLD, TS_UPGRADE, TS_FROZEN};

class BaseTower: public Selectable, public Hills::Recipient
{
    public:
        friend class TowerManager;
		
        // Attacking methods
        void GetTargetsInRange();
		void Attack(Ogre::Real p_rTimeSinceLastFrame);
        void SetPriorityTarget(BaseEnemy* p_pEnemy);

        // Upgrade this tower to the next level
		virtual void Upgrade();

        // Salvage this tower, destroying it and receiving a portion of its cost back
        virtual void Salvage();

        // Stop this tower from attacking until it is unfrozen by a player
        virtual void Freeze();
        virtual void Unfreeze();

        // Getters for upgrade cost and 
        inline Currency GetUpgradeCost(int p_iLevel) { return m_cCost[p_iLevel]; }
        inline int GetLevel() { return m_iLevel; }

        // Check methods
        inline bool CanDelete() { return m_bCanDelete; }
        inline bool CanInteract() { return m_bCanInteract; }
        inline bool HasPriorityTarget() { return m_bHasPriorityTarget; }

        // State methods
        inline TowerStates GetState() { return m_eState; }
        inline void SetState(TowerStates p_eState) { m_eState = p_eState; }

        // Clean up OGRE-related data
        void DestroyModel();

        // Update the tower each frame based on its state
		virtual void Update(const Ogre::FrameEvent& p_feEvent);
		
        // Getters for various values
        inline Ogre::SceneNode* GetSceneNode() { return m_pModel->GetSceneNode(); }
		inline Ogre::Vector3 GetLocation() { return m_pModel->GetLocation(); }
        inline BaseEnemy* GetPriorityTarget() { return m_pPriorityTarget; }
        inline Tile* GetTile() { return m_pTile; }

        // Handle selection by selection circle with a size parameter
        void Select();

		/***********************************************************************************
		*	Added by Pierre
		************************************************************************************/
		virtual const Ogre::AxisAlignedBox& GetBoundingBox() { return m_pBoundingBox->_getWorldAABB(); } 
		virtual void ShowObject(bool p_bIsShow);

		// Handle event method, each subclass has their own way to take care of an event.
		virtual void HandleEvent(const Hills::GameEvent& p_geGameEvent);
		/***********************************************************************************/
        
    protected:
		// Tower related variables
		float m_fRange, m_fAtkRate, m_fDamage;
        int m_iLevel;
        Currency m_cCost[5];
        TowerStates m_eState;
        float m_fSellTimeLeft, m_fBuildTimeLeft;
        bool m_bCanInteract, m_bUpgradeReady, m_bCanDelete, m_bAnimationPlaying;

        // Night logic
        float m_fRangeModifier;

        // Selection circle 
        Tile* m_pTile;
        float m_fCircleSize, m_fCircleOffset;

        // Projectile stuff
        Ogre::Vector3 m_v3ProjectileOffset;
        float m_fProjectileSpeed;

        // Particle effects
        Ogre::ParticleSystem* m_pParticleSystem;
        std::string m_sParticleName;
        float m_fParticleTime;
        bool m_bParticleActive;
        
        // Target related variables
        std::vector<BaseEnemy*> m_vEnemies;
        BaseEnemy* m_eCurrentTarget;
        BaseEnemy* m_pPriorityTarget;
        float m_fTimeUntilNextAtk;
        int m_iCurrentTarget;
        bool m_bHasPriorityTarget, m_bPriorityTargetInRange;

        //Sound related variables
        int m_iChannelNumber;
		
		// Ogre related variables
		Model* m_pModel, *m_pModel2;
        Ogre::SceneNode* m_pBoundingBox;

        Ogre::ParticleSystem* m_pBuildEffect;
        Ogre::SceneNode* m_pDustNode;
        
        // Protected constructor so class can not be initialized normally
        BaseTower();
        BaseTower(float p_fRange, float p_fAtkRate, float p_fDamage, float p_fBuildTime,
                  std::string p_sMesh, Ogre::SceneManager* p_smManager);
        ~BaseTower();
};

#endif