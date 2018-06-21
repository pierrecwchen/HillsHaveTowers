/************************************************************************
GameState.h										Created by Pavel Szczesny

State class which houses the main game.
************************************************************************/
#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "BaseState.h"
#include "..\EventManager.h"
#include "..\SoundManager.h"
#include "..\ProjectileManager.h"
#include "..\EnemyWaveSender.h"
#include "..\EnemyManager.h"
#include "..\TowerManager.h"
#include "..\VillagerManager.h"
#include "..\ResourceManager.h"
#include "..\TileSet.h"
#include "..\LevelParser.h"
#include "..\Player.h"
#include "..\Terrain.h"
#include "..\MainCurrency.h"
#include "..\Currency.h"
#include "..\SelectionCircle.h"
#include "..\ShadowListener.h"

#include "..\IntroDemon.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <time.h>

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>
#include <SdkCameraMan.h>

namespace Hills
{
	class GameState : public BaseState
	{
	public:
		/************************************************************************
		Public Functions
		************************************************************************/
		GameState();
		~GameState();
		virtual void Init();
		virtual void Update(const Ogre::FrameEvent& p_evtDelta);
		virtual void Destroy();

		// OIS::KeyListener
		virtual bool KeyPressed( const OIS::KeyEvent& pArg );
		virtual bool KeyReleased( const OIS::KeyEvent& pArg );
		// OIS::MouseListener
		virtual bool MouseMoved( const OIS::MouseEvent& pArg );
		virtual bool MousePressed( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId );
		virtual bool MouseReleased( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId );

		static BaseState* Create() { return new GameState(); }

		void SetPause(bool p_bPause = true)
		{
			m_bPause = p_bPause;
			SelectableManager::SetPause(p_bPause);
		}

    private:
		/************************************************************************
		Private Functions
		************************************************************************/
        void GenerateLevel();
		void GenerateTerrain();
		void ReadPathFile(std::string p_sName);
        void GenerateResources();
		
		/************************************************************************
		Private Members
		************************************************************************/
		bool m_bPause;
		int** m_iTerrainLayer;
	    int** m_iPathLayer;
	    int** m_iResourceLayer;

        Player* m_pPlayer;
        EnemyWaveSender* m_ewsWaves;
        Ogre::SceneNode* m_pDayNode;
        Ogre::SceneNode* m_pNightNode;

        bool m_bCutscenePlaying;
        IntroDemon* m_pIntro;

        bool m_bLight, m_bFireActive,m_bCircleActive, m_bAttackPlaying;

		// Light movement variables
		float m_fAngle;
		const float m_fRotationSpeed;
		const float m_fRadius;
		const Ogre::Vector3 m_vCenter;

        // TEST
        Ogre::MaterialPtr mat, mat2;
        Ogre::ManualObject* obj;
        Model* demonModel;
        Demon* m_pDemon;
        Ogre::ParticleSystem* demonFire, *demonFire2;
        float fireTime, demonTime;
        int fireLevel;

		Ogre::Root* m_pRoot;
        Ogre::Light *m_pLevelLight;
		Ogre::Light* m_pNightLight;
		Ogre::Camera* m_pCamera;
        Ogre::Vector3 m_vCameraPos;
		Ogre::SceneManager* m_pSceneManager;
	};
}
#endif