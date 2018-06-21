/************************************************************************
TerrainTestState.cpp							Created by Pavel Szczesny

State class for testing out all thing terrain related
************************************************************************/

#include "TerrainTestState.h"
#include "..\BaseApplication.h"
#include "..\Terrain.h"

namespace Hills
{

	/************************************************************************
	Constructor
	************************************************************************/
	TerrainTestState::TerrainTestState()
		: m_pRoot(0),
		m_pSceneManager(0),
		m_pCamera(0)
	{
	
	}

	/************************************************************************
	Constructor
	************************************************************************/
	TerrainTestState::~TerrainTestState()
	{
		
	}

	/************************************************************************
	Init

	Setup scene, camera, lights, terrain, and any necessary materials
	************************************************************************/
	void TerrainTestState::Init()
	{
		UIManager::SwitchWindows("LoadingScreen");
		LoadingScreen::GetSingletonPtr()->ResetProgressBar();

		m_pRoot = Ogre::Root::getSingletonPtr();

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.0f);

		m_pSceneManager = m_pRoot->getSceneManager(BaseApplication::GetSceneManagerName());
		m_pCamera = m_pSceneManager->getCamera("PlayerCam");

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.05f);

		//m_pCamera->setPosition(Ogre::Vector3(101, 200, 100));
		//m_pCamera->lookAt(Ogre::Vector3(100, 0, 100));
		m_pCamera->setPosition(Ogre::Vector3(128.0f, 187.0f, 340.0f));
		m_pCamera->lookAt(Ogre::Vector3(128.0f, 0.0f, 144.0f));
		m_pSceneManager->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.1f));

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.05f);

		/*m_pSpotlight = m_pSceneManager->createLight("SpotLight");
		m_pSpotlight->setType(Ogre::Light::LT_SPOTLIGHT);
		m_pSpotlight->setDiffuseColour(Ogre::ColourValue(0.5, 0.5, 0.5));
		m_pSpotlight->setPosition(m_pCamera->getPosition());
		m_pSpotlight->setDirection(m_pCamera->getDirection());
		m_pSpotlight->setSpotlightRange(Ogre::Degree(45), Ogre::Degree(45));
		m_pSpotlight->setSpotlightFalloff(100);*/

		/*Ogre::Light* pDirectionalLight = m_pSceneManager->createLight("DirectionalLight");
		pDirectionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
		pDirectionalLight->setDiffuseColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
		pDirectionalLight->setDirection(Ogre::Vector3(1.0f, -1.0f, 0.0f));*/

		Ogre::Light* pPointLight = m_pSceneManager->createLight("PointLight");
		pPointLight->setType(Ogre::Light::LT_POINT);
		pPointLight->setDiffuseColour(Ogre::ColourValue(0.0f, 1.0f, 0.0f));
		pPointLight->setPosition(Ogre::Vector3(256.0f, 20.0f, 256.0f));
		pPointLight->setAttenuation(100.0f, 1.0f, 1.0f, 0.5f);

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.1f);

		Terrain::GenerateTerrain("hmlevel1.png", 1);

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.5f);

		Ogre::Entity* pTerrain = m_pSceneManager->createEntity("Terrain", "Grid");
		pTerrain->setMaterialName("HillsHaveTowers/TerrainTest");
		m_pSceneManager->getRootSceneNode()->createChildSceneNode("GridNode")->attachObject(pTerrain);

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.3f);

		UIManager::SwitchWindows("InGameHUD");
	}

	/************************************************************************
	Update
	************************************************************************/
	void TerrainTestState::Update(const Ogre::FrameEvent& p_evtDelta)
	{
		//m_pSpotlight->setPosition(m_pCamera->getPosition());
		//m_pSpotlight->setDirection(m_pCamera->getDirection());
	}

	/************************************************************************
	Destroy

	Delete terrain and clean up
	************************************************************************/
	void TerrainTestState::Destroy()
	{
		Terrain::Destroy();

		m_pSceneManager->destroyAllEntities();
		m_pSceneManager->destroyAllMovableObjects(); // Double check
		//m_pSceneManager->destroyAllAnimations();
		//m_pSceneManager->destroyAllAnimationStates();
		//m_pSceneManager->destroySceneNode("GridNode");
		m_pSceneManager->getRootSceneNode()->removeAndDestroyAllChildren();
		m_pSceneManager->destroyAllLights();
		m_pSceneManager->destroyAllManualObjects();
		//m_pSceneManager->destroyAllParticleSystems();

		// All these only store pointers.
		m_pRoot = 0;
		m_pSceneManager = 0;
		m_pCamera = 0;
		m_pSpotlight = 0;
	}

	/************************************************************************
	KeyPressed

	Determine action when key is pressed
	************************************************************************/
	bool TerrainTestState::KeyPressed( const OIS::KeyEvent& pArg )
	{
		if (pArg.key == OIS::KC_ESCAPE)
		{
			InGameHUD::GetSingletonPtr()->PauseGame();
		}

		return true;
	}

	/************************************************************************
	KeyReleased

	Determine action when key is released
	************************************************************************/
	bool TerrainTestState::KeyReleased( const OIS::KeyEvent& pArg )
	{
		return true;
	}

	/************************************************************************
	MouseMoved

	Determine action when mouse is moved
	************************************************************************/
	bool TerrainTestState::MouseMoved( const OIS::MouseEvent& pArg )
	{
		return true;
	}

	/************************************************************************
	MousePressed

	Determine action when mouse is pressed
	************************************************************************/
	bool TerrainTestState::MousePressed( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId )
	{
		
		return true;
	}

	/************************************************************************
	MouseReleased

	Determine action when mouse is released
	************************************************************************/
	bool TerrainTestState::MouseReleased( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId )
	{
		/*float temp = Terrain::GetHeight(200.0f, 50.0f);
		if (temp >= 0)
		{
			int n = temp;
		}*/
		return true;
	}
}