/************************************************************************
GameState.cpp									Created by Pavel Szczesny

State class which houses the main game. While created by Pavel, the class
has been filled out by...
************************************************************************/

#include "GameState.h"

#include "..\GameDataManager.h"
#include "..\SceneryManager.h"
#include "..\AdrenalinePower.h"
#include "..\MeteorPower.h"

namespace Hills
{
    GameState::GameState() :
	    m_bPause(false),
		m_pRoot(0),
		m_pCamera(0),
		m_pSceneManager(0),
        m_bCutscenePlaying(false),
		m_fAngle(0.0f),
		m_fRotationSpeed(0.5f),
		m_fRadius(500.0f),
		m_vCenter(Ogre::Vector3(256.0f, 0.0f, 256.0f)),
		m_pNightLight(NULL)
	{
	
	}

	GameState::~GameState()
	{
		
	}

	void GameState::Init()
	{
		m_bInitializing = true;

		srand(time(NULL));

		UIManager::SwitchWindows("LoadingScreen");
		LoadingScreen::GetSingletonPtr()->ResetProgressBar();

		//------------------------------------------------------------------------------
		// Added by Pavel Szczesny
		// Get the current root so we can grab the default scene manager that was 
		// initialized in BaseApplication. This will then allow us to grab the default
		// camera called "PlayerCam"
		m_pRoot = Ogre::Root::getSingletonPtr();

		m_pSceneManager = m_pRoot->getSceneManager(BaseApplication::GetSceneManagerName());

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.05f);

		m_pCamera = m_pSceneManager->getCamera("PlayerCam");
        m_pCamera->setFarClipDistance(1000);
		//------------------------------------------------------------------------------

        LevelParser::ReadLevelFile("../Media/levels/level1.xml");
        //LevelParser::ReadLevelFile("../Media/levels/waterLevel.xml");
		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.05f);

        GenerateLevel();

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.25f);
        
        //LevelParser::ReadFile("../Media/levels/level1.txt");

        m_vCameraPos = Ogre::Vector3(147.0f + Tile::GetXOffset(), 197.0f, 342.0f + Tile::GetZOffset());
        Ogre::Vector3 target(147.0f + Tile::GetXOffset(), 0.0f, 154.0f + Tile::GetZOffset());

		m_pCamera->setPosition(m_vCameraPos);
		m_pCamera->lookAt(target);

        m_iTerrainLayer = LevelParser::GetTerrainLayer();
        m_iResourceLayer = LevelParser::GetResourceLayer();

        TileSet::GenerateTiles(m_iTerrainLayer, m_iResourceLayer, LevelParser::GetNumTilesX(), LevelParser::GetNumTilesZ());  // Generate the tiles for the level
        TileSet::FindShortestPath();

        LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.25f);

        GenerateResources();

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.1f);

		VillagerManager::CreateVillage();

#if 1
        
        // Turn self-shadowing on
		m_pSceneManager->setShadowTextureSelfShadow(true);
        // Set our caster and receiver materials
        m_pSceneManager->setShadowTextureCasterMaterial("HillsHaveTowers/Caster");
        // Set the pixel format to floating point
        m_pSceneManager->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_R);
        //m_pSceneManager->setShadowTexturePixelFormat(Ogre::PF_FLOAT32_R);
		m_pSceneManager->setShadowCasterRenderBackFaces(false);
        // Finally enable the shadows using texture additive integrated
		m_pSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
		//m_pSceneManager->setShadowColour(Ogre::ColourValue(0.1f, 0.1f, 0.1f));

        Ogre::ShadowCameraSetupPtr mCurrentShadowCameraSetup = Ogre::ShadowCameraSetupPtr(new Ogre::FocusedShadowCameraSetup());
        m_pSceneManager->setShadowCameraSetup(mCurrentShadowCameraSetup);

        m_pSceneManager->setShadowTextureSize(1024);
        
#endif
		
        // Light setup
        m_pSceneManager->setAmbientLight(Ogre::ColourValue(0.01f,0.01f,0.01f));
        
        //Ogre::Light* directionalLight = m_pSceneManager->createLight("directionalLight");
        //directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
        //directionalLight->setDiffuseColour(Ogre::ColourValue(0.7f, 0.75f, 0.8f));
        ////directionalLight->setSpecularColour(Ogre::ColourValue(0.9f, 0.9f, 0.9f));
        //directionalLight->setDirection(Ogre::Vector3(1.0f, -1.0f, -1.0f));
        
		// Create the day light
        m_pLevelLight = m_pSceneManager->createLight("flashLight");
        m_pLevelLight->setDiffuseColour(Ogre::ColourValue(0.7f, 0.75f, 0.8f));
        m_pLevelLight->setType(Ogre::Light::LT_SPOTLIGHT);
        m_pLevelLight->setSpotlightInnerAngle(Ogre::Degree(25));
        m_pLevelLight->setSpotlightOuterAngle(Ogre::Degree(45));
		//m_pLevelLight->setSpotlightFalloff(1.0f);
        m_pLevelLight->setAttenuation(1200, 1, 0, 0); // meter range.  the others our shader ignores
        m_pLevelLight->setPosition(25, 303, 450);
        m_pLevelLight->setDirection(Ogre::Vector3(1.0f, -1.0f, -1.0f));
        m_pDayNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode("flashLightNode");
        m_pDayNode->attachObject(m_pLevelLight);

		// Create the night light and position it oposite of the day light
		m_pNightLight = m_pSceneManager->createLight("NightLight");
        m_pNightLight->setDiffuseColour(Ogre::ColourValue(0.26f, 0.35f, 0.5f));
        //m_pNightLight->setDiffuseColour(Ogre::ColourValue(0.06f, 0.05f, 0.05f));
        m_pNightLight->setType(Ogre::Light::LT_SPOTLIGHT);
        m_pNightLight->setSpotlightInnerAngle(Ogre::Degree(25));
        m_pNightLight->setSpotlightOuterAngle(Ogre::Degree(45));
        m_pNightLight->setAttenuation(1200, 1, 0, 0); // meter range.  the others our shader ignores
		m_pNightLight->setPosition(Ogre::Vector3(-m_pLevelLight->getPosition().x, -m_pLevelLight->getPosition().y, m_pLevelLight->getPosition().z));
        m_pNightLight->setDirection(Ogre::Vector3(1.0f, -1.0f, -1.0f));
        m_pNightNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode("NightLightNode");
        m_pNightNode->attachObject(m_pNightLight);

        m_ewsWaves = LevelParser::GetWaveSender();
        m_pPlayer = new Player("dwarf.mesh");

        m_pPlayer->AddPower(new AdrenalinePower(m_pPlayer));
        m_pPlayer->AddPower(new MeteorPower());

        //TEST
        /*Ogre::RibbonTrail* mSwordTrail = (Ogre::RibbonTrail*)m_pSceneManager->createMovableObject("RibbonTrail");
		mSwordTrail->setTrailLength(20);
		mSwordTrail->setVisible(true);
		m_pSceneManager->getRootSceneNode()->attachObject(mSwordTrail);
        mSwordTrail->setNumberOfChains(1);
        mSwordTrail->setMaxChainElements(80);
        mSwordTrail->setInitialColour(0, 1, 1, 1);
		mSwordTrail->setColourChange(0, 0, 0, 0, 1.25);
		mSwordTrail->setWidthChange(0, 2);
		mSwordTrail->setInitialWidth(0, 1.5);
        mSwordTrail->addNode(m_pPlayer->GetModel()->GetSceneNode());
        //mSwordTrail->setMaterialName("M_Transparency");
        TEST*/
        MainCurrency::SetCurrency(500,0,0,0);

		TileMenu::GetSingletonPtr()->SetPlayer(m_pPlayer);
		TowerMenu::GetSingletonPtr()->SetPlayer(m_pPlayer);

        SoundManager::PositionListener(m_vCameraPos, m_pCamera->getDirection());
        SoundManager::InitGameSounds();

		LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.1f);

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

            LoadingScreen::GetSingletonPtr()->IncreaseProgressBar(0.01f);

            getline(file, line);
        }

		UIManager::SwitchWindows("InGameHUD");

        //Ogre::CompositorManager::getSingleton().addCompositor(m_pCamera->getViewport(), "Outline");
        //Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_pCamera->getViewport(), "Outline", true);

        TowerManager::Instance()->Add(T_MAGIC, TileSet::GetTileAt(9,12));
        TowerManager::Instance()->Add(T_TESLA, TileSet::GetTileAt(10,12));
        TowerManager::Instance()->Add(T_TORCH, TileSet::GetTileAt(11,12));

        SoundManager::PlayMusic("secondSuite.wav");

		m_bInitializing = false;
	}

	void GameState::Update(const Ogre::FrameEvent& p_evtDelta)
	{
        if(m_bPause)
			return;

        // Have to check it because the "renderOneFrame" method.
		if(m_bInitializing)
			return;

        if (m_bCutscenePlaying)
        {
            if (m_pIntro->Playing())
                m_pIntro->Update(p_evtDelta);
            else
            {
                SoundManager::PositionListener(m_pCamera->getPosition(),
                                               m_pCamera->getDirection());
                delete m_pIntro;
                m_pIntro = 0;
                m_bCutscenePlaying = false;
            }
        }

#if 0
		//------------------------------------------------------------
		// Day / Night light cycle
		// Written by Pavel Szczesny
		//------------------------------------------------------------
		// Get the current z position to maintain it during light movement
		float fZPosition = m_pLevelLight->getPosition().z;
		// Move the day light about the z axis
		Ogre::Vector3 vNewPosition = Ogre::Vector3(m_fRadius * sin(m_fAngle), m_fRadius * cos(m_fAngle), fZPosition);
		m_fAngle += m_fRotationSpeed * p_evtDelta.timeSinceLastFrame;
		m_pLevelLight->setPosition(vNewPosition);
		// Reset the direction of the light
		Ogre::Vector3 vDirection = m_vCenter - vNewPosition;
		vDirection.normalise();
		m_pLevelLight->setDirection(vDirection);

		// Move the night light to oposite position of the day light about the z axis
		Ogre::Vector3 vNightPosition = Ogre::Vector3(-vNewPosition.x, -vNewPosition.y, vNewPosition.z);
		m_pNightLight->setPosition(vNightPosition);
		// Reset the direction of the light
		Ogre::Vector3 vNightDirection = m_vCenter - vNightPosition;
		vNightDirection.normalise();
		m_pNightLight->setDirection(vNightDirection);

		// When the lights are below the terrain, turn them off
		/*if (m_pLevelLight->getPosition().y < 0.0f)
		{
			m_pLevelLight->setVisible(false);
		}
		else
		{
			m_pLevelLight->setVisible(true);
		}
		if (m_pNightLight->getPosition().y < 0.0f)
		{
			m_pNightLight->setVisible(false);
		}
		else
		{
			m_pNightLight->setVisible(true);
		}*/
		// End of Pavel's code for Day / Night cycle
		//------------------------------------------------------------
#endif

		EventManager::BroadCast();
        m_pPlayer->Update(p_evtDelta);
        VillagerManager::Update(p_evtDelta);
        EnemyManager::Instance()->Update(p_evtDelta);
		TowerManager::Instance()->Update(p_evtDelta);
        ResourceManager::Update(p_evtDelta);
        ProjectileManager::Update(p_evtDelta);
        SelectionCircle::Update();
		// Toggle waves on/off by commenting out line
        //m_ewsWaves->Update(p_evtDelta);
		UIManager::Update();
	}

    void GameState::GenerateLevel()
	{
        Terrain::GenerateTerrain(LevelParser::GetHeightMapFileName(), LevelParser::GetTerrainXStretch());
		Ogre::Entity* pTerrain = m_pSceneManager->createEntity("Terrain", "Grid");
        pTerrain->setCastShadows(false);
		pTerrain->setMaterialName(LevelParser::GetMaterialName());
        pTerrain->setQueryFlags(QF_TERRAIN);

        //Ogre::SceneNode* pNode2 = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
        //pNode2->translate(140,60,200);
        //Ogre::ParticleSystem* p1x = m_pSceneManager->createParticleSystem("test1x", "Snowflakes");
        //pNode2->attachObject(p1x);
        //Ogre::ParticleSystem* p2x = m_pSceneManager->createParticleSystem("test2x", "ImpactBlue");
        //pNode2->attachObject(p2x);
        //Ogre::ParticleSystem* p3x = m_pSceneManager->createParticleSystem("test3x", "SnowBlast");
        //pNode2->attachObject(p3x);
        //Ogre::ParticleSystem* p4x = m_pSceneManager->createParticleSystem("test4x", "Smoketrail");
        //pNode2->attachObject(p4x);
        //Ogre::ParticleSystem* p5x = m_pSceneManager->createParticleSystem("test5x", "Explosion2");
        //pNode2->attachObject(p5x);
        //Ogre::ParticleSystem* p6 = m_pSceneManager->createParticleSystem("test6x", "SmokePuff");
        //pNode2->attachObject(p6);

		m_pSceneManager->getRootSceneNode()->createChildSceneNode("GridNode")->attachObject(pTerrain);
        //m_pRoot->renderOneFrame(); // Render the terrain so that raycasting can be done on it

        SelectionCircle::Initialize("decal.png");
	}

    void GameState::GenerateResources()
    {
        // Place resources on the proper tiles
		for(int k = 0; k < LevelParser::GetNumTilesZ(); ++k)
		{
			for(int j = 0; j < LevelParser::GetNumTilesX(); ++j)
			{
                if (m_iResourceLayer[j][k] == 1)
                    ResourceManager::Add(WoodResource, TileSet::GetTileAt(j,k), 5);
                else if (m_iResourceLayer[j][k] == 2)
                    ResourceManager::Add(StoneResource, TileSet::GetTileAt(j,k), 5);
                else if (m_iResourceLayer[j][k] == 3)
                    ResourceManager::Add(IronResource, TileSet::GetTileAt(j,k), 5);
			}
		}
    }

	void GameState::Destroy()
	{
        SetPause(false);

        LevelParser::Destroy();

		TileSet::Destroy();

		if(m_pPlayer)
			delete m_pPlayer;

		Terrain::Destroy();

		EnemyBar::GetSingletonPtr()->Reset();

		GameDataManager::Destroy();
		VillagerManager::DeleteAll();
		EnemyManager::Instance()->DeleteAll();
		TowerManager::Instance()->DeleteAll();
        ResourceManager::DeleteAll();
        ProjectileManager::DeleteAll();
		EventManager::RemoveAllEvent();
		EventManager::RemoveAllRecipients();
		SelectableManager::RemoveAll(); // Double check
        SelectionCircle::Destroy();
        Village::Destroy();

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

	bool GameState::KeyPressed( const OIS::KeyEvent& pArg )
	{
		if (!m_bCutscenePlaying)
        {
            if(m_bPause)
			    return true;
            
            if (pArg.key == OIS::KC_ESCAPE)
		    {
			    if(m_bPause)
				    PauseMenu::GetSingletonPtr()->ResumeGame();
			    else
				    InGameHUD::GetSingletonPtr()->PauseGame();
		    }

            if(pArg.key == OIS::KC_LSHIFT)
            {
                float xPos = Tile::GetXOffset() + TileSet::GetWidth() * Tile::GetDefaultSize() / 2;
                float zPos = Tile::GetZOffset() + TileSet::GetLength() * Tile::GetDefaultSize() / 2;

                Ogre::Vector3 position(xPos, 320, zPos);
                Ogre::Vector3 target(xPos, 0, zPos - 1);

                m_pCamera->setPosition(position);
		        m_pCamera->lookAt(target);

                SoundManager::PositionListener(position, m_pCamera->getDirection());
            }
            if(pArg.key == OIS::KC_1)  // Shorten the path (1)
            {
                ResourceManager::Delete(16,13);
            }
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
            else if (pArg.key == OIS::KC_C)
            {
                m_pPlayer->PreparePower(0);
            }
            else if (pArg.key == OIS::KC_V)
            {
                m_pPlayer->PreparePower(1);
            }
            else if (pArg.key == OIS::KC_Y)
            {
                EnemyManager::Instance()->Add(ET_DEMON);
            }
		    else if (pArg.key == OIS::KC_H)  // Harvest selected resource (Test only)
            {
                if(SelectableManager::GetCurrentSelectedObj() != 0 &&
				    SelectableManager::GetCurrentSelectedObj()->GetSelectableObjType() == SO_Resource)
			    {
				    dynamic_cast<BaseResource*>(SelectableManager::GetCurrentSelectedObj())->Harvest();
			    }
            }
            else if (pArg.key == OIS::KC_8)
            {
                MainCurrency::SetCurrency(10000, 99, 99, 99);
            }
            else if (pArg.key == OIS::KC_6)
            {
                m_pSceneManager->setShadowCasterRenderBackFaces(false);
            }
		    else if (pArg.key == OIS::KC_7)
            {
                m_pSceneManager->setShadowCasterRenderBackFaces(true);
            }
            else if (pArg.key == OIS::KC_9)
            {
                //RESERVED FOR NICK'S SOUND TESTING
                SoundManager::Play3DPitchedSound("bowShot.wav", Ogre::Vector3(300, 0, 300), 0.5);
            }
            else if (pArg.key == OIS::KC_Z)
            {
                m_pIntro = new IntroDemon();
                m_bCutscenePlaying = true;
            }
        }

        return true;
	}

	bool GameState::KeyReleased( const OIS::KeyEvent& pArg )
	{
        if(pArg.key == OIS::KC_LSHIFT)
        {
            Ogre::Vector3 position(147.0f + Tile::GetXOffset(), 197.0f, 342.0f + Tile::GetZOffset());
            Ogre::Vector3 target(147.0f + Tile::GetXOffset(), 0.0f, 154.0f + Tile::GetZOffset());

            m_pCamera->setPosition(position);
		    m_pCamera->lookAt(target);

            SoundManager::PositionListener(position, m_pCamera->getDirection());
        }

		if(m_bPause)
			return true;

		return true;
	}

	bool GameState::MouseMoved( const OIS::MouseEvent& pArg )
	{
		if(m_bPause)
			return true;

		// Update current pointing object.
        if (!m_bCutscenePlaying)
		    SelectableManager::UpdateCurrentPointingObj(pArg);

        return true;
	}

	bool GameState::MousePressed( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId )
	{
		if(m_bPause)
			return true;

        if (!m_bCutscenePlaying)
        {
		    // Update current selected object
		    SelectableManager::UpdateCurrentSelectedObj(pArg);

            if (SelectableManager::GetCurrentSelectedObj() != 0) // Check that there is an object selected
            {
			
			    TileMenu::GetSingletonPtr()->SetTileToMove(0);
			    TowerMenu::GetSingletonPtr()->SetTower(0);

			    //TileMenu::GetSingletonPtr()->PushToBack();
			    //TowerMenu::GetSingletonPtr()->PushToBack();
                if(m_pPlayer->ExpectingSelectable())
                {
                    if (pArg.state.buttonDown(OIS::MB_Left))
                    {
                        m_pPlayer->ActivatePower(SelectableManager::GetCurrentSelectedObj());
                    }
                    else
                    {
                        m_pPlayer->CancelPower();
                    }
                }
                //---------------------------------------------------------------------------
                // Check if the selected object is a tile
                //---------------------------------------------------------------------------
                else if (SelectableManager::GetCurrentSelectedObj()->GetSelectableObjType() == SO_Tile)
                {
                    if (pArg.state.buttonDown(OIS::MB_Left)) // Left-click to move to and build on the tile
                    {
                        // If the tower has a priority target, we want to display the targetting circle on it
                        if (dynamic_cast<Tile*>(SelectableManager::GetCurrentSelectedObj())->IsBuildable())
                        {
						    TileMenu::GetSingletonPtr()->SetTileToMove(dynamic_cast<Tile*>(SelectableManager::GetCurrentSelectedObj()));
                        }

                        TowerManager::Instance()->SetSelectedTower(0);
                        SelectionCircle::HideTargetCircle();
                        SelectionCircle::HideSelectionCircle();
                    }
                    else if (pArg.state.buttonDown(OIS::MB_Right)) // Right-click to move to the tile
                    {
                        m_pPlayer->MoveTo(dynamic_cast<Tile*>(SelectableManager::GetCurrentSelectedObj()));

                        TowerManager::Instance()->SetSelectedTower(0);
                        SelectionCircle::HideTargetCircle();
                        SelectionCircle::HideSelectionCircle();

                        m_pPlayer->SetImpendingAction(PA_NONE);
                    }
                }
                //---------------------------------------------------------------------------
                // Check if the selected object is a resource
                //---------------------------------------------------------------------------
                if (SelectableManager::GetCurrentSelectedObj()->GetSelectableObjType() == SO_Resource)
                {
                    if (pArg.state.buttonDown(OIS::MB_Left)) // Left-click to select a resource
                    {
                        dynamic_cast<BaseResource*>(SelectableManager::GetCurrentSelectedObj())->Select();
                        TowerManager::Instance()->SetSelectedTower(0);
                        SelectionCircle::HideTargetCircle();
                    }
                    else if (pArg.state.buttonDown(OIS::MB_Right)) // Right-click to move to and harvest it
                    {
                        dynamic_cast<BaseResource*>(SelectableManager::GetCurrentSelectedObj())->Select();
                    
                        m_pPlayer->MoveTo(dynamic_cast<BaseResource*>(SelectableManager::GetCurrentSelectedObj())->GetTile());
                        m_pPlayer->SetResourceToHarvest(dynamic_cast<BaseResource*>(SelectableManager::GetCurrentSelectedObj()));

                        TowerManager::Instance()->SetSelectedTower(0);
                        SelectionCircle::HideTargetCircle();
                        SelectionCircle::HideSelectionCircle();
                        m_pPlayer->SetImpendingAction(PA_HARVEST);
                    }
                }
                //---------------------------------------------------------------------------
                // Check if the selected object is a tower
                //---------------------------------------------------------------------------
                if (SelectableManager::GetCurrentSelectedObj()->GetSelectableObjType() == SO_Tower)
                {
                    if (pArg.state.buttonDown(OIS::MB_Left)) // Left-click to select a tower
                    {
                        // Set the tower as the current selected tower, if an enemy is right-clicked as the next selection
                        // it will be set as this tower's priority target
                        dynamic_cast<BaseTower*>(SelectableManager::GetCurrentSelectedObj())->Select();
                        TowerManager::Instance()->SetSelectedTower(dynamic_cast<BaseTower*>(SelectableManager::GetCurrentSelectedObj()));
                        if (dynamic_cast<BaseTower*>(SelectableManager::GetCurrentSelectedObj())->HasPriorityTarget())
                        {
                            SelectionCircle::LockTarget(dynamic_cast<BaseTower*>(SelectableManager::GetCurrentSelectedObj())->
                                                                                 GetPriorityTarget()->GetSceneNode());
                        }
                        else
                            SelectionCircle::HideTargetCircle();
					    TowerMenu::GetSingletonPtr()->SetTower(dynamic_cast<BaseTower*>(SelectableManager::GetCurrentSelectedObj()));
                    }
                    else if (pArg.state.buttonDown(OIS::MB_Right)) // Right-click to move to it
                    {
                        m_pPlayer->MoveTo(dynamic_cast<BaseTower*>(SelectableManager::GetCurrentSelectedObj())->GetTile());

                        // If the tower is broken, we want to run over and repair it
                        if (dynamic_cast<BaseTower*>(SelectableManager::GetCurrentSelectedObj())->GetState() == TS_FROZEN)
                        {
                            //TowerMenu::GetSingletonPtr()->SetTower(dynamic_cast<BaseTower*>(SelectableManager::GetCurrentSelectedObj()));
                            m_pPlayer->SetTowerToUpgrade(dynamic_cast<BaseTower*>(SelectableManager::GetCurrentSelectedObj()));
                            m_pPlayer->SetImpendingAction(PA_REPAIR);
                        }
                        else
                        {
                            SelectionCircle::HideSelectionCircle();
                            m_pPlayer->SetImpendingAction(PA_NONE);
                        }

                        TowerManager::Instance()->SetSelectedTower(0);
                        SelectionCircle::HideTargetCircle();
                    }
                }
                //---------------------------------------------------------------------------
                // Check if the selected object is an enemy
                //---------------------------------------------------------------------------
                if (SelectableManager::GetCurrentSelectedObj()->GetSelectableObjType() == SO_Enemy)
                {
                    if (pArg.state.buttonDown(OIS::MB_Left)) // Left-click to select an enemy
                    {
                        dynamic_cast<BaseEnemy*>(SelectableManager::GetCurrentSelectedObj())->Select();
                        TowerManager::Instance()->SetSelectedTower(0);
                        SelectionCircle::HideTargetCircle();
                    }
                    else if (pArg.state.buttonDown(OIS::MB_Right)) // Right-click to set the enemy as a priority target for a tower
                    {
                        // Check if there is a tower selected
                        if (TowerManager::Instance()->GetSelectedTower() != 0)
                        {
                            TowerManager::Instance()->GetSelectedTower()->SetPriorityTarget(dynamic_cast<BaseEnemy*>(SelectableManager::GetCurrentSelectedObj()));
                            SelectionCircle::LockTarget(dynamic_cast<BaseEnemy*>(SelectableManager::GetCurrentSelectedObj())->GetSceneNode());
                        }

                        TowerManager::Instance()->SetSelectedTower(0);
                    }
                }

			    TileMenu::GetSingletonPtr()->MoveMenuTo(pArg);
			    TowerMenu::GetSingletonPtr()->MoveMenuTo(pArg);
            }

		    if(pArg.state.buttonDown(OIS::MB_Right))
			    TowerMenu::GetSingletonPtr()->CheckMouseCursor();
        }
        return true;
	}

	bool GameState::MouseReleased( const OIS::MouseEvent& pArg, OIS::MouseButtonID pId )
	{
		if(m_bPause)
			return true;

		return true;
	}
}