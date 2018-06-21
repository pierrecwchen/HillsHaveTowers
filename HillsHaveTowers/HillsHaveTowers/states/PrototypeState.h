/************************************************************************
PrototypeState.h								Created by Pavel Szczesny

State class which houses the game prototype.
************************************************************************/
#ifndef PROTOTYPESTATE_H
#define PROTOTYPESTATE_H

#include "BaseState.h"
#include "..\EventManager.h"
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
	class PrototypeState : public BaseState
	{
	public:
		/************************************************************************
		Public Functions
		************************************************************************/
		PrototypeState();
		~PrototypeState();
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

		static BaseState* Create() { return new PrototypeState(); }

		void SetPause(bool p_bPause = true){ m_bPause = p_bPause; }

	private:
		/************************************************************************
		Private Functions
		************************************************************************/
		void GenerateHardLevel();
		void GenerateTerrain();
		void ReadPathFile(std::string p_sName);
        //void GenerateResources();

		// temp function for drawing a cube
		void DrawCube();
		
		/************************************************************************
		Private Members
		************************************************************************/
		bool m_bPause;
		int** m_iTerrainLayer;
	    int** m_iPathLayer;
	    int** m_iResourceLayer;

		//TileSet* m_pTiles;
		//BaseEnemy* m_pEnemy;
		//BaseTower* m_pTower;
        Player* m_pPlayer;
        EnemyWaveSender* m_ewsWaves;

		Ogre::Root* m_pRoot;
		Ogre::Camera* m_pCamera;
		Ogre::SceneManager* m_pSceneManager;
	};
}
#endif