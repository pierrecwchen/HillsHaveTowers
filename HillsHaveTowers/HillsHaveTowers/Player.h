//-----------------------------------------------------------------------------
// Player.h                                                 By: Keenan Johnston
// 
// Represents the player character within the game.  Moves around and interacts
// on the level through player input.
//
// Base code by Keenan, pathfinding code by Nick, other things as noted.
//-----------------------------------------------------------------------------

#ifndef PLAYER_H
#define PLAYER_H

#include "Recipient.h"
#include "GameEvent.h"
#include "Selectable.h"
#include "Model.h"
#include "Tile.h"
#include "TowerManager.h"
#include "ResourceManager.h"
#include "SoundManager.h"
#include "MainCurrency.h"
#include "BasePower.h"

enum PlayerStates {PlayerIdleState, PlayerWalkState, PlayerHarvestState, PlayerKnockdownState, PlayerRejectState, PS_REPAIRING};
enum PlayerActions {PA_NONE, PA_BUILD, PA_HARVEST, PA_UPGRADE, PA_REPAIR};
enum PlayerAnimations {AS_IDLE, AS_CHOP, AS_MINE, AS_DEATH, AS_WALK, AS_SPIN, AS_NO};
enum TowerToBuild {TTB_ArrowTower, TTB_CannonTower, TTB_BurstTower, TTB_SniperTower, TTB_SlowTower};

class Player : public Hills::Recipient
{
    public:
        // Constructor and destructor
        Player(std::string p_sMesh);
        ~Player();

        // Tell player the location to move to, getting there handled by pathfinding code
        void MoveTo(Tile* p_pTile);

        // Flags to determine what action to perform upon reaching a tile
        inline void SetImpendingAction(PlayerActions p_eAction) { m_eAction = p_eAction; }

        //inline void ToHarvest(bool p_bToHarvest) { m_bToHarvest = p_bToHarvest; }
        //inline void ToBuild(bool p_bToBuild) { m_bToBuild = p_bToBuild; }
        //inline void ToUpgrade(bool p_bToUpgrade) { m_bToUpgrade = p_bToUpgrade; }

        // Setters corresponding to the actions of the above three methods
        inline void SetResourceToHarvest(BaseResource* p_pResource) { m_pResourceToHarvest = p_pResource; }
        inline void SetTowerToBuild(TowerToBuild p_eTower) { m_eTower = p_eTower; }
        inline void SetTowerToUpgrade(BaseTower* p_pTower) { m_pTowerToUpgrade = p_pTower; }

        inline void SetSpeedMultiplier(float p_fSpeed) { m_fSpeedMultiplier = p_fSpeed; m_pModel->SetAnimationSpeed(p_fSpeed * 0.5f); }

        // Actons corresponding to the above three methods to be performed when destination is reached
        void Harvest(BaseResource* p_pResource);
        void BuildTower(Towers p_eType, Tile* p_tTile);
        void UpgradeTower(BaseTower* p_pTower);
        
        // Getters for a few useful values
        inline PlayerStates GetState() { return m_eState; }
        inline Model* GetModel() { return m_pModel; }
        inline Ogre::Vector3 GetLocation() { return m_pModel->GetLocation(); }

        bool ExpectingSelectable() { return m_iCurrentPower != -1; }

        void AddPower(BasePower* p_bpNewPower) { m_vPowers.push_back(p_bpNewPower); }

        void PreparePower(int p_iIndex);
        void ActivatePower(Selectable* p_sTarget);
        void CancelPower() { m_iCurrentPower = -1; }

        // Update each frame based on the current state
        void Update(const Ogre::FrameEvent& p_feEvent);

        /***********************************************************************************
		*	Added by Pierre
		************************************************************************************/

		// Handle event method, each subclass has their own way to take care of an event.
		virtual void HandleEvent(const Hills::GameEvent& p_geGameEvent);
		/***********************************************************************************/
        
    private:
        // Physical attributes of the player
        float m_fMoveSpeed, m_fBuildSpeed, m_fHarvestSpeed, m_fSpeedMultiplier;
        int m_iHeight;

        // Timers for player stun effect and resource harvesting
        float m_fStunTime, m_fImmunityTime, m_fBlinkTime;
        float m_fTimeUntilNextHarvest, m_fRepairTime;

        // Flags for player action when tile is reached
        PlayerActions m_eAction;
        //bool m_bToHarvest, m_bToBuild, m_bToUpgrade;

        // Concrete values corresponding to above three flags
        BaseResource* m_pResourceToHarvest;
        TowerToBuild m_eTower;
        BaseTower* m_pTowerToUpgrade, *m_pTowerToRepair;

        // State variables
        PlayerStates m_eState;
        PlayerAnimations m_eAnimState;

        // Player's model
        Model* m_pModel;

        //NICK~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
        //Helper methods for finding the orientation to the next tile
        float GetAngleDifference(float p_fCurrent, float p_fDestination);
        float GetAngleToNextTile();

        //Finds which tile the player is to be headed towards
        void FindTargetTile();
        
        //RayCasting method which finds out if the player can walk to that tile
        bool CanWalkFromTo(Ogre::Vector3 p_v3Start, Ogre::Vector3 p_v3Destination);

        void PlayHarvestSound();
        void PlayRejectSound();

        //The final tile the player is heading for and his current tile
        Tile* m_pDestination;
        Tile m_tCurrentTile;

        //The position the player is currently heading for and the one he's ultimately heading for
        Ogre::Vector3 m_v3TargetLocation;
        Ogre::Vector3 m_v3FinalLocation;

        //The player's path
        std::list<Ogre::Vector3> m_lCurrentPath;

        bool m_bMadeHarvestNoise;

        std::vector<BasePower*> m_vPowers;

        int m_iCurrentPower;
        //~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
};

#endif