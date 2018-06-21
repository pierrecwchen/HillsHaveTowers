/************************************************************************
PrototypeState.cpp								Created by Pavel Szczesny

State class for the game prototype. While created by Pavel, the class
has been filled out by...
************************************************************************/

#include "PrototypeState.h"
#include "..\BaseApplication.h"

namespace Hills
{
	/************************************************************************
	Constructor
	************************************************************************/
    PrototypeState::PrototypeState()
		: //m_pEnemy(0),
		//m_pTower(0),
		//m_pTiles(0),
		m_pRoot(0),
		m_pCamera(0),
		m_pSceneManager(0),
		m_bPause(false)
	{
	
	}

	/************************************************************************
	Destructor
	************************************************************************/
	PrototypeState::~PrototypeState()
	{
		
	}

	/************************************************************************
	Init
	************************************************************************/
	void PrototypeState::Init()
	{
		m_bInitializing = true;
        
		//------------------------------------------------------------------------------
		// Copy from game state
		//------------------------------------------------------------------------------
		UIManager::SwitchWindows("LoadingScreen");
		LoadingScreen::GetSingletonPtr()->ResetProgressBar();

		//------------------------------------------------------------------------------
		// Added by Pavel Szczesny
		// Get the current root so we can grab the default scene manager that was 
		// initialized in BaseApplication. This will then allow us to grab the default
		// camera called "PlayerCam"
		m_pRoot = Ogre::Root::getSingletonPtr();

		m_pSceneManager = m_pRoot->getSceneManager(BaseApplication::GetSceneManagerName());

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.05);

		m_pCamera = m_pSceneManager->getCamera("PlayerCam");
		//------------------------------------------------------------------------------

		m_pCamera->setPosition(Ogre::Vector3(128.0f, 187.0f, 340.0f));
		m_pCamera->lookAt(Ogre::Vector3(128.0f, 0.0f, 144.0f));

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.05);

        GenerateHardLevel();

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.1);

        // Shadow setup
        //m_pSceneManager->setShadowTextureSize(1024);
        //m_pSceneManager->setShadowColour(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
        //m_pSceneManager->setShadowTextureFSAA(4);
        //m_pSceneManager->setShadowCasterRenderBackFaces(false);
        //m_pSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);

        m_pSceneManager->setShadowTextureSelfShadow(true);
        m_pSceneManager->setShadowTextureCasterMaterial("Ogre/DepthShadowmap/Caster/Float");
        m_pSceneManager->setShadowTexturePixelFormat(Ogre::PF_FLOAT32_R);
        m_pSceneManager->setShadowCasterRenderBackFaces(false);
        m_pSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);

        // Light setup
        m_pSceneManager->setAmbientLight(Ogre::ColourValue(0.1f,0.1f,0.1f));
        Ogre::Light* directionalLight = m_pSceneManager->createLight("directionalLight");
        directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
        directionalLight->setDiffuseColour(Ogre::ColourValue(0.97f, 0.97f, 1.0f));
        directionalLight->setSpecularColour(Ogre::ColourValue(0.9f, 0.9f, 0.9f));
        directionalLight->setDirection(Ogre::Vector3(0.5f, -1.0f, -1.0f));
        /*
        m_ewsWaves = LevelParser::GetWaveSender();
        m_pPlayer = new Player("dwarf.mesh");

		TileMenu::GetSingletonPtr()->SetPlayer(m_pPlayer);
		TowerMenu::GetSingletonPtr()->SetPlayer(m_pPlayer);

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.1);

        srand(time(NULL));

        Ogre::SceneNode* preNode;
        ifstream file;
        string line;

        file.open("meshes.txt", std::ios::in);
        getline(file, line);

        while (line.at(0) != '#')
        {
            Ogre::Entity* preEntity = m_pSceneManager->createEntity(line);
            preNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
            preNode->attachObject(preEntity);

            preNode->getCreator()->destroyMovableObject(preEntity);
    
            preNode->removeAndDestroyAllChildren();
            preNode->getParentSceneNode()->removeAndDestroyChild(preNode->getName());

            LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.01);

            getline(file, line);
        }

        // Delete enemies
        //EnemyManager::Instance()->DeleteAll();
        */
		UIManager::SwitchWindows("InGameHUD");
        
		m_bInitializing = false;
	}

	/************************************************************************
	Update
	************************************************************************/
	void PrototypeState::Update(const Ogre::FrameEvent& p_evtDelta)
	{
		if(m_bPause)
			return;

		// Have to check it because the "renderOneFrame" method.
		if(m_bInitializing)
			return;

		EventManager::BroadCast();
        //m_pPlayer->Update(p_evtDelta);
        VillagerManager::Update(p_evtDelta);
        EnemyManager::Instance()->Update(p_evtDelta);
		TowerManager::Instance()->Update(p_evtDelta);
        ResourceManager::Update(p_evtDelta);
        ProjectileManager::Update(p_evtDelta);
		// Toggle waves on/off by commenting out line
        //m_ewsWaves->Update(p_evtDelta);
	}

	/************************************************************************
	Destroy
	************************************************************************/
	void PrototypeState::Destroy()
	{
		SetPause(false);

		LevelParser::Destroy();

		TileSet::Destroy();

		if(m_pPlayer)
			delete m_pPlayer;

		Terrain::Destroy();

		EnemyBar::GetSingletonPtr()->Reset();

		VillagerManager::DeleteAll();
		EnemyManager::Instance()->DeleteAll();
		TowerManager::Instance()->DeleteAll();
        ResourceManager::DeleteAll();
        ProjectileManager::DeleteAll();
		EventManager::RemoveAllEvent();
		EventManager::RemoveAllRecipients();
		SelectableManager::RemoveAll(); // Double check

		m_pSceneManager->destroyAllEntities();
		m_pSceneManager->destroyAllMovableObjects(); // Double check
		m_pSceneManager->destroyAllAnimations();
		m_pSceneManager->destroyAllAnimationStates();
		//m_pSceneManager->destroySceneNode("GridNode");
		m_pSceneManager->getRootSceneNode()->removeAndDestroyAllChildren();
		m_pSceneManager->destroyAllLights();
		m_pSceneManager->destroyAllManualObjects();
		m_pSceneManager->destroyAllRibbonTrails();
		m_pSceneManager->destroyAllParticleSystems();

		// All these only store pointers.
		m_pRoot = 0;
		m_pSceneManager = 0;
		m_pCamera = 0;
		m_ewsWaves = 0;
		m_iTerrainLayer = 0;
		m_iResourceLayer = 0;
	}

	/************************************************************************
	DrawCube (by Pavel)

	Function for drawing a cube using Ogre's Manual Object class
	************************************************************************/
	void PrototypeState::DrawCube()
	{
		Ogre::ManualObject* pCube = new Ogre::ManualObject("BoundingCube");
		pCube->begin("", Ogre::RenderOperation::OT_LINE_LIST);
		// line 1
		pCube->position(0, 0, 0);
		pCube->position(1, 0, 0);
		// line 2
		pCube->position(1, 1, 0);
		pCube->position(0, 1, 0);
		// line 3
		pCube->position(0, 0, 0);
		pCube->position(0, 1, 0);
		// line 4
		pCube->position(1, 0, 0);
		pCube->position(1, 1, 0);
		// line 5
		pCube->position(0, 0, 1);
		pCube->position(1, 0, 1);
		// line 6
		pCube->position(0, 1, 1);
		pCube->position(1, 1, 1);
		// line 7
		pCube->position(0, 0, 1);
		pCube->position(0, 1, 1);
		// line 8
		pCube->position(1, 0, 1);
		pCube->position(1, 1, 1);
		// line 9
		pCube->position(0, 0, 0);
		pCube->position(0, 0, 1);
		// line 10
		pCube->position(1, 0, 0);
		pCube->position(1, 0, 1);
		// line 11
		pCube->position(0, 1, 0);
		pCube->position(0, 1, 1);
		// line 12
		pCube->position(1, 1, 0);
		pCube->position(1, 1, 1);

		pCube->end();
		
		// Vertices and indexes for a solid cube using a triangle list to render
		/*pCube->begin("", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		pCube->position(0, 0, 0);
		pCube->position(0, 0, 1);
		pCube->position(0, 1, 1);
		pCube->position(0, 1, 0);
		
		pCube->position(0, 0, 1);
		pCube->position(1, 0, 1);
		pCube->position(1, 0, 0);
		
		pCube->position(1, 0, 1);
		pCube->position(1, 1, 1);
		pCube->position(0, 1, 1);
		
		pCube->position(0, 1, 1);
		pCube->position(1, 1, 1);*/

		//pCube->position(0, 0, 0); // 0
		//pCube->position(1, 0, 0); // 1
		//pCube->position(0, 1, 0); // 2
		//pCube->position(1, 1, 0); // 3
		//pCube->position(0, 0, 1); // 4
		//pCube->position(1, 0, 1); // 5
		//pCube->position(0, 1, 1); // 6
		//pCube->position(1, 1, 1); // 7

		//pCube->triangle(0, 1, 2);
		//pCube->triangle(2, 1, 3);
		//pCube->triangle(2, 5, 3);
		//pCube->triangle(3, 5, 7);
		//pCube->triangle(0, 4, 2);
		//pCube->triangle(2, 4, 6);
		//pCube->triangle(0, 1, 4);
		//pCube->triangle(4, 1, 5);
		//pCube->triangle(2, 3, 6);
		//pCube->triangle(6, 3, 7);
		//pCube->triangle(4, 5, 6);
		//pCube->triangle(6, 5, 7);
		//pCube->end();

		pCube->convertToMesh("CubeMesh");

		Ogre::MaterialPtr pMaterial = Ogre::MaterialManager::getSingleton().create("Test/CubeTest", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		pMaterial->getTechnique(0)->getPass(0)->setVertexColourTracking(Ogre::TrackVertexColourEnum::TVC_AMBIENT);

		Ogre::Entity* pCubeEntity = m_pSceneManager->createEntity("TestCube", "CubeMesh");
		pCubeEntity->setMaterialName("Test/CubeTest");
		m_pSceneManager->getRootSceneNode()->createChildSceneNode("CubeNode")->attachObject(pCube);
		// Change camera to see cube at the origin. Only use this for testing
		//m_pCamera->setPosition(Ogre::Vector3(0, 5, 5));
		//m_pCamera->lookAt(0, 0, 0);
	}
	
	/************************************************************************
	GenerateHardLevel
	************************************************************************/
	void PrototypeState::GenerateHardLevel()
	{
		//A plane is created, placed and assigned the hard level texture
		Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

		Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			plane, 100, 100, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

		Ogre::Entity* entGround = m_pSceneManager->createEntity("ground");
		m_pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(entGround);
	    entGround->setMaterialName("HillsHaveTowers/Terrain");
		entGround->setCastShadows(false);

        Ogre::Entity* preEntity = m_pSceneManager->createEntity("soldier.mesh");
        Ogre::SceneNode* preNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
        preNode->attachObject(preEntity);
        /*
        Terrain::GenerateTerrain();
		Ogre::Entity* pTerrain = m_pSceneManager->createEntity("Terrain", "Grid");
		pTerrain->setMaterialName("HillsHaveTowers/Terrain");
		m_pSceneManager->getRootSceneNode()->createChildSceneNode("GridNode")->attachObject(pTerrain);
        //m_pRoot->renderOneFrame();

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.25);
        
        Tile::SetDefaultSize(12.8);
        LevelParser::ReadFile("level1.txt");
        m_iTerrainLayer = LevelParser::GetTerrainLayer();
        m_iResourceLayer = LevelParser::GetResourceLayer();

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.25);

        TileSet::SetTiles(LevelParser::GetTiles(), 20, 20);  // Generate the tiles for the level
        TileSet::FindShortestPath();

		// Place resources on the proper tiles
		for(int k = 0; k < 20; ++k)
		{
			for(int j = 0; j < 20; ++j)
			{
                if (m_iResourceLayer[j][k] == 1)
                    ResourceManager::Add(WoodResource, TileSet::GetTileAt(j,k), 5);
                else if (m_iResourceLayer[j][k] == 2)
                    ResourceManager::Add(StoneResource, TileSet::GetTileAt(j,k), 5);
                else if (m_iResourceLayer[j][k] == 3)
                    ResourceManager::Add(IronResource, TileSet::GetTileAt(j,k), 5);

			}
		}

        //TileSet::FindShortestPath();
        // Path needs to be set before enemies can be created

        // Test some of the other towers here until the player is able to build them
        //TowerManager::Instance()->Add(eSniperTower, TileSet::GetTileAt(5,15));
        */
	}
    /*
	void PrototypeState::GenerateTerrain()
	{
		Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

		Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			plane, 100, 100, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

		TileSet* tiles = new TileSet(p);

		//For every single tile, either a grass plane is generated or a path terrain is generated. Functionality could also be
		// added for the starting tile, the path-blocked tiles and the village tile.
		for(int k = 0; k < 20; ++k)
		{
			for(int j = 0; j < 20; ++j)
			{
				if(m_cPathLayer[j][k] == 0)
				{
					Ogre::Entity* entGround = m_pSceneManager->createEntity(Ogre::StringConverter::toString(k+j*20), "ground");
					m_pSceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-j*100, -100, -k*100))->attachObject(entGround);
					entGround->setMaterialName("Examples/GrassFloor");
					entGround->setCastShadows(false);
				}
				else
				{
					Ogre::Entity* entGround = m_pSceneManager->createEntity(Ogre::StringConverter::toString(k+j*20), "ground");
					m_pSceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-j*100, -100, -k*100))->attachObject(entGround);
					entGround->setMaterialName("Examples/Rockwall");
					entGround->setCastShadows(false);
				}
			}
		}
		m_pTiles = new TileSet(m_iPathLayer);
	}
    */
	/************************************************************************
	KeyPressed
	************************************************************************/
	bool PrototypeState::KeyPressed( const OIS::KeyEvent &pArg )
	{
		if (pArg.key == OIS::KC_ESCAPE)
		{
			if(m_bPause)
				PauseMenu::GetSingletonPtr()->ResumeGame();
			else
				InGameHUD::GetSingletonPtr()->PauseGame();
		}

		if(m_bPause)
			return true;

        if(pArg.key == OIS::KC_1)  // Shorten the path (1)
        {
            ResourceManager::Delete(16,13);        
        }
        else if(pArg.key == OIS::KC_2)  // Shorten the path (2)
        {
            ResourceManager::Delete(13,7);
            ResourceManager::Delete(13,9);
            ResourceManager::Delete(13,11);
        }
        else if(pArg.key == OIS::KC_3)  // Shorten the path (3)
        {
            ResourceManager::Delete(7,7);
            ResourceManager::Delete(7,11);
        }
        else if (pArg.key == OIS::KC_K)  // Kill selected enemy (Test only)
        {
			if(SelectableManager::GetCurrentSelectedObj() != 0 &&
				SelectableManager::GetCurrentSelectedObj()->GetSelectableObjType() == SO_Enemy)
			{
				dynamic_cast<BaseEnemy*>(SelectableManager::GetCurrentSelectedObj())->SetHP(0);
			}
        }
        else if (pArg.key == OIS::KC_U)  // Add a new soldier
        {
            EnemyManager::Instance()->Add(SoldierEnemy);
        }
        else if (pArg.key == OIS::KC_I)  // Add a new scout
        {
            EnemyManager::Instance()->Add(ScoutEnemy);
        }
        else if (pArg.key == OIS::KC_O)  // Add a new priest
        {
            EnemyManager::Instance()->Add(PriestEnemy);
        }
        else if (pArg.key == OIS::KC_P)  // Add a new knight
        {
            EnemyManager::Instance()->Add(KnightEnemy);
        }
		else if (pArg.key == OIS::KC_H)  // Harvest selected resource (Test only)
        {
            if(SelectableManager::GetCurrentSelectedObj() != 0 &&
				SelectableManager::GetCurrentSelectedObj()->GetSelectableObjType() == SO_Resource)
			{
				dynamic_cast<BaseResource*>(SelectableManager::GetCurrentSelectedObj())->Harvest();
			}
        }
        return true;
	}

	/************************************************************************
	KeyReleased
	************************************************************************/
	bool PrototypeState::KeyReleased( const OIS::KeyEvent& pArg )
	{
		if(m_bPause)
			return false;

		return true;
	}

	/************************************************************************
	MouseMoved
	************************************************************************/
	bool PrototypeState::MouseMoved( const OIS::MouseEvent& pArg )
	{
		if(m_bPause)
			return false;

		// Update current pointing object.
		SelectableManager::UpdateCurrentPointingObj(pArg);

		return true;
	}

	/************************************************************************
	MousePressed
	************************************************************************/
	bool PrototypeState::MousePressed( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId )
	{
		if(m_bPause)
			return true;

		// Update current selected object
		SelectableManager::UpdateCurrentSelectedObj(pArg);

        if (SelectableManager::GetCurrentSelectedObj() != 0) // Check that there is an object selected
        {
			TileMenu::GetSingletonPtr()->SetTileToMove(0);
			TowerMenu::GetSingletonPtr()->SetTower(0);

			//TileMenu::GetSingletonPtr()->PushToBack();
			//TowerMenu::GetSingletonPtr()->PushToBack();

            if (SelectableManager::GetCurrentSelectedObj()->GetSelectableObjType() == SO_Tile) // Check if the selected object is a tile
            {
                if (pArg.state.buttonDown(OIS::MB_Left)) // Left-click to move to and build on the tile
                {
                    if (dynamic_cast<Tile*>(SelectableManager::GetCurrentSelectedObj())->IsBuildable())
                    {
						TileMenu::GetSingletonPtr()->SetTileToMove(dynamic_cast<Tile*>(SelectableManager::GetCurrentSelectedObj()));

                        //m_pPlayer->MoveTo(dynamic_cast<Tile*>(SelectableManager::GetCurrentSelectedObj()));
                        //m_pPlayer->ToBuild(true);
                        //m_pPlayer->ToHarvest(false);
                        //m_pPlayer->ToUpgrade(false);  // reset all flags if player had a previous selection
                        //m_pPlayer->MoveTo(dynamic_cast<Tile*>(SelectableManager::GetCurrentSelectedObj()));

                        //m_pPlayer->ToBuild(true);
                        //m_pPlayer->ToHarvest(false);
                        //m_pPlayer->ToUpgrade(false);  // reset all flags if player had a previous selection
                    }

                    TowerManager::Instance()->SetSelectedTower(0);
                }
                else if (pArg.state.buttonDown(OIS::MB_Right)) // Right-click to move to the tile
                {
                    m_pPlayer->MoveTo(dynamic_cast<Tile*>(SelectableManager::GetCurrentSelectedObj()));

                    TowerManager::Instance()->SetSelectedTower(0);
                    //m_pPlayer->ToBuild(false);
                    //m_pPlayer->ToHarvest(false);
                    //m_pPlayer->ToUpgrade(false);  // reset all flags if player had a previous selection
                }
            }
            if (SelectableManager::GetCurrentSelectedObj()->GetSelectableObjType() == SO_Resource)
            {
                if (pArg.state.buttonDown(OIS::MB_Left)) // Left-click to select a resource
                {
                    TowerManager::Instance()->SetSelectedTower(0);
                }
                else if (pArg.state.buttonDown(OIS::MB_Right)) // Right-click to move to and harvest it
                {
                    m_pPlayer->MoveTo(dynamic_cast<BaseResource*>(SelectableManager::GetCurrentSelectedObj())->GetTile());
                    m_pPlayer->SetResourceToHarvest(dynamic_cast<BaseResource*>(SelectableManager::GetCurrentSelectedObj()));

                    TowerManager::Instance()->SetSelectedTower(dynamic_cast<BaseTower*>(SelectableManager::GetCurrentSelectedObj()));
                    //m_pPlayer->ToHarvest(true);
                    //m_pPlayer->ToBuild(false);
                    //m_pPlayer->ToUpgrade(false);  // reset all flags if player had a previous selection
                }
            }
            if (SelectableManager::GetCurrentSelectedObj()->GetSelectableObjType() == SO_Tower)
            {
                if (pArg.state.buttonDown(OIS::MB_Left)) // Left-click to select a tower
                {
                    // Set the tower as the current selected tower, if an enemy is right-clicked as the next selection
                    // it will be set as this tower's priority target
                    TowerManager::Instance()->SetSelectedTower(dynamic_cast<BaseTower*>(SelectableManager::GetCurrentSelectedObj()));
					
					TowerMenu::GetSingletonPtr()->SetTower(dynamic_cast<BaseTower*>(SelectableManager::GetCurrentSelectedObj()));
                }
                else if (pArg.state.buttonDown(OIS::MB_Right)) // Right-click to move to and bring up the tower menu
                {
                    m_pPlayer->MoveTo(dynamic_cast<BaseTower*>(SelectableManager::GetCurrentSelectedObj())->GetTile());
                    //m_pPlayer->SetTowerToUpgrade(dynamic_cast<BaseTower*>(SelectableManager::GetCurrentSelectedObj()));

                    TowerManager::Instance()->SetSelectedTower(dynamic_cast<BaseTower*>(SelectableManager::GetCurrentSelectedObj()));
                    //m_pPlayer->ToHarvest(false);
                    //m_pPlayer->ToBuild(false);
                    //m_pPlayer->ToUpgrade(true);  // reset all flags if player had a previous selection
                }
            }
            if (SelectableManager::GetCurrentSelectedObj()->GetSelectableObjType() == SO_Enemy)
            {
                if (pArg.state.buttonDown(OIS::MB_Left)) // Left-click to select an enemy
                {
                    TowerManager::Instance()->SetSelectedTower(0);
                }
                else if (pArg.state.buttonDown(OIS::MB_Right)) // Right-click to set the enemy as a priority target for a tower
                {
                    // Check if there is a tower selected
                    if (TowerManager::Instance()->GetSelectedTower() != 0)
                        TowerManager::Instance()->GetSelectedTower()->SetPriorityTarget(dynamic_cast<BaseEnemy*>(SelectableManager::GetCurrentSelectedObj()));

                    TowerManager::Instance()->SetSelectedTower(0);
                }
            }
        }

		TileMenu::GetSingletonPtr()->MoveMenuTo(pArg);
		TowerMenu::GetSingletonPtr()->MoveMenuTo(pArg);

        return true;
	}

	/************************************************************************
	MouseReleased
	************************************************************************/
	bool PrototypeState::MouseReleased( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId )
	{
		if(m_bPause)
			return false;

        //float temp = Terrain::GetHeight(125, 125);

		return true;
	}
}