/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#include "stdafx.h"
#include "TutorialApplication.h"

//-------------------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
{
}
//-------------------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{
}

//-------------------------------------------------------------------------------------
void TutorialApplication::createScene(void)
{
    p = new int*[20];

    for(int j = 0; j < 20; j++)
    {
        p[j] = new int[20];
    }

     p[0][0] = 0;  p[1][0] = 0;  p[2][0] = 0;  p[3][0] = 0;  p[4][0] = 0;  p[5][0] = 0;  p[6][0] = 0;  p[7][0] = 0;  p[8][0] = 0;  p[9][0] = 0;  p[10][0] = 0;  p[11][0] = 0;  p[12][0] = 0;  p[13][0] = 0;  p[14][0] = 0;  p[15][0] = 0;  p[16][0] = 0;  p[17][0] = 0;  p[18][0] = 0;  p[19][0] = 0;
     p[0][1] = 0;  p[1][1] = 0;  p[2][1] = 0;  p[3][1] = 0;  p[4][1] = 0;  p[5][1] = 0;  p[6][1] = 0;  p[7][1] = 0;  p[8][1] = 0;  p[9][1] = 0;  p[10][1] = 0;  p[11][1] = 0;  p[12][1] = 0;  p[13][1] = 0;  p[14][1] = 0;  p[15][1] = 0;  p[16][1] = 0;  p[17][1] = 0;  p[18][1] = 0;  p[19][1] = 0;
     p[0][2] = 0;  p[1][2] = 0;  p[2][2] = 0;  p[3][2] = 0;  p[4][2] = 0;  p[5][2] = 0;  p[6][2] = 0;  p[7][2] = 0;  p[8][2] = 0;  p[9][2] = 0;  p[10][2] = 0;  p[11][2] = 0;  p[12][2] = 0;  p[13][2] = 0;  p[14][2] = 0;  p[15][2] = 0;  p[16][2] = 0;  p[17][2] = 0;  p[18][2] = 4;  p[19][2] = 0;
     p[0][3] = 0;  p[1][3] = 0;  p[2][3] = 0;  p[3][3] = 0;  p[4][3] = 0;  p[5][3] = 0;  p[6][3] = 0;  p[7][3] = 0;  p[8][3] = 0;  p[9][3] = 0;  p[10][3] = 0;  p[11][3] = 0;  p[12][3] = 0;  p[13][3] = 0;  p[14][3] = 0;  p[15][3] = 0;  p[16][3] = 0;  p[17][3] = 0;  p[18][3] = 1;  p[19][3] = 0;
     p[0][4] = 0;  p[1][4] = 0;  p[2][4] = 0;  p[3][4] = 0;  p[4][4] = 0;  p[5][4] = 0;  p[6][4] = 0;  p[7][4] = 0;  p[8][4] = 0;  p[9][4] = 0;  p[10][4] = 0;  p[11][4] = 0;  p[12][4] = 0;  p[13][4] = 0;  p[14][4] = 0;  p[15][4] = 0;  p[16][4] = 0;  p[17][4] = 1;  p[18][4] = 1;  p[19][4] = 0;
     p[0][5] = 0;  p[1][5] = 0;  p[2][5] = 0;  p[3][5] = 1;  p[4][5] = 1;  p[5][5] = 1;  p[6][5] = 1;  p[7][5] = 1;  p[8][5] = 1;  p[9][5] = 1;  p[10][5] = 1;  p[11][5] = 1;  p[12][5] = 1;  p[13][5] = 1;  p[14][5] = 1;  p[15][5] = 1;  p[16][5] = 1;  p[17][5] = 1;  p[18][5] = 0;  p[19][5] = 0;
     p[0][6] = 0;  p[1][6] = 0;  p[2][6] = 1;  p[3][6] = 1;  p[4][6] = 0;  p[5][6] = 0;  p[6][6] = 0;  p[7][6] = 1;  p[8][6] = 0;  p[9][6] = 0;  p[10][6] = 0;  p[11][6] = 0;  p[12][6] = 0;  p[13][6] = 1;  p[14][6] = 0;  p[15][6] = 0;  p[16][6] = 0;  p[17][6] = 0;  p[18][6] = 0;  p[19][6] = 0;
     p[0][7] = 0;  p[1][7] = 1;  p[2][7] = 1;  p[3][7] = 0;  p[4][7] = 0;  p[5][7] = 0;  p[6][7] = 0;  p[7][7] = 2;  p[8][7] = 0;  p[9][7] = 0;  p[10][7] = 0;  p[11][7] = 0;  p[12][7] = 0;  p[13][7] = 2;  p[14][7] = 0;  p[15][7] = 0;  p[16][7] = 0;  p[17][7] = 0;  p[18][7] = 0;  p[19][7] = 0;
     p[0][8] = 0;  p[1][8] = 1;  p[2][8] = 0;  p[3][8] = 0;  p[4][8] = 0;  p[5][8] = 0;  p[6][8] = 0;  p[7][8] = 1;  p[8][8] = 0;  p[9][8] = 0;  p[10][8] = 0;  p[11][8] = 0;  p[12][8] = 0;  p[13][8] = 1;  p[14][8] = 0;  p[15][8] = 0;  p[16][8] = 0;  p[17][8] = 0;  p[18][8] = 0;  p[19][8] = 0;
     p[0][9] = 0;  p[1][9] = 1;  p[2][9] = 0;  p[3][9] = 0;  p[4][9] = 0;  p[5][9] = 0;  p[6][9] = 0;  p[7][9] = 2;  p[8][9] = 0;  p[9][9] = 0;  p[10][9] = 0;  p[11][9] = 0;  p[12][9] = 0;  p[13][9] = 1;  p[14][9] = 0;  p[15][9] = 0;  p[16][9] = 0;  p[17][9] = 0;  p[18][9] = 0;  p[19][9] = 0;
    p[0][10] = 0; p[1][10] = 1; p[2][10] = 0; p[3][10] = 0; p[4][10] = 0; p[5][10] = 0; p[6][10] = 0; p[7][10] = 1; p[8][10] = 0; p[9][10] = 0; p[10][10] = 0; p[11][10] = 0; p[12][10] = 0; p[13][10] = 1; p[14][10] = 0; p[15][10] = 0; p[16][10] = 0; p[17][10] = 0; p[18][10] = 0; p[19][10] = 0;
    p[0][11] = 0; p[1][11] = 1; p[2][11] = 1; p[3][11] = 0; p[4][11] = 0; p[5][11] = 0; p[6][11] = 0; p[7][11] = 2; p[8][11] = 0; p[9][11] = 0; p[10][11] = 0; p[11][11] = 0; p[12][11] = 0; p[13][11] = 1; p[14][11] = 0; p[15][11] = 0; p[16][11] = 0; p[17][11] = 0; p[18][11] = 0; p[19][11] = 0;
    p[0][12] = 0; p[1][12] = 0; p[2][12] = 1; p[3][12] = 1; p[4][12] = 0; p[5][12] = 0; p[6][12] = 0; p[7][12] = 1; p[8][12] = 0; p[9][12] = 0; p[10][12] = 0; p[11][12] = 0; p[12][12] = 0; p[13][12] = 1; p[14][12] = 0; p[15][12] = 0; p[16][12] = 0; p[17][12] = 0; p[18][12] = 0; p[19][12] = 0;
    p[0][13] = 0; p[1][13] = 0; p[2][13] = 0; p[3][13] = 1; p[4][13] = 1; p[5][13] = 1; p[6][13] = 1; p[7][13] = 1; p[8][13] = 1; p[9][13] = 1; p[10][13] = 1; p[11][13] = 1; p[12][13] = 1; p[13][13] = 1; p[14][13] = 1; p[15][13] = 1; p[16][13] = 2; p[17][13] = 1; p[18][13] = 1; p[19][13] = 3;
    p[0][14] = 0; p[1][14] = 0; p[2][14] = 0; p[3][14] = 0; p[4][14] = 0; p[5][14] = 0; p[6][14] = 0; p[7][14] = 0; p[8][14] = 0; p[9][14] = 0; p[10][14] = 0; p[11][14] = 0; p[12][14] = 0; p[13][14] = 1; p[14][14] = 0; p[15][14] = 0; p[16][14] = 0; p[17][14] = 1; p[18][14] = 0; p[19][14] = 0;
    p[0][15] = 0; p[1][15] = 0; p[2][15] = 0; p[3][15] = 0; p[4][15] = 0; p[5][15] = 0; p[6][15] = 0; p[7][15] = 0; p[8][15] = 0; p[9][15] = 0; p[10][15] = 0; p[11][15] = 0; p[12][15] = 0; p[13][15] = 1; p[14][15] = 0; p[15][15] = 0; p[16][15] = 0; p[17][15] = 1; p[18][15] = 0; p[19][15] = 0;
    p[0][16] = 0; p[1][16] = 0; p[2][16] = 0; p[3][16] = 0; p[4][16] = 0; p[5][16] = 0; p[6][16] = 0; p[7][16] = 0; p[8][16] = 0; p[9][16] = 0; p[10][16] = 0; p[11][16] = 0; p[12][16] = 0; p[13][16] = 1; p[14][16] = 0; p[15][16] = 0; p[16][16] = 0; p[17][16] = 1; p[18][16] = 0; p[19][16] = 0;
    p[0][17] = 0; p[1][17] = 0; p[2][17] = 0; p[3][17] = 0; p[4][17] = 0; p[5][17] = 0; p[6][17] = 0; p[7][17] = 0; p[8][17] = 0; p[9][17] = 0; p[10][17] = 0; p[11][17] = 0; p[12][17] = 0; p[13][17] = 1; p[14][17] = 1; p[15][17] = 1; p[16][17] = 1; p[17][17] = 1; p[18][17] = 0; p[19][17] = 0;
    p[0][18] = 0; p[1][18] = 0; p[2][18] = 0; p[3][18] = 0; p[4][18] = 0; p[5][18] = 0; p[6][18] = 0; p[7][18] = 0; p[8][18] = 0; p[9][18] = 0; p[10][18] = 0; p[11][18] = 0; p[12][18] = 0; p[13][18] = 0; p[14][18] = 0; p[15][18] = 0; p[16][18] = 0; p[17][18] = 0; p[18][18] = 0; p[19][18] = 0;
    p[0][19] = 0; p[1][19] = 0; p[2][19] = 0; p[3][19] = 0; p[4][19] = 0; p[5][19] = 0; p[6][19] = 0; p[7][19] = 0; p[8][19] = 0; p[9][19] = 0; p[10][19] = 0; p[11][19] = 0; p[12][19] = 0; p[13][19] = 0; p[14][19] = 0; p[15][19] = 0; p[16][19] = 0; p[17][19] = 0; p[18][19] = 0; p[19][19] = 0;

    
    mCamera->setPosition(Ogre::Vector3(-950.0f, 1200.0f, -2400.0f));
    mCamera->lookAt(Ogre::Vector3(-950.0f, 0.0f, -1325.0f));

    //mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
    //Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
    //pointLight->setType(Ogre::Light::LT_POINT);
    //pointLight->setPosition(Ogre::Vector3(-1020, 50, -1000));
    
    generateHardLevel();
}

//Generates a single plane that's textured with a map of the level we plan to design. It limits our ability to create
// our own terrain for experimentation, however, it runs much faster
void TutorialApplication::generateHardLevel()
{
    //A plane is created, placed and assigned the hard level texture
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

    Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 2000, 2000, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

    Ogre::Entity* entGround = mSceneMgr->createEntity("ground");
    mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-950, -100, -950))->attachObject(entGround);
    entGround->setMaterialName("Examples/HardLevel");
    entGround->setCastShadows(false);

    //Ogre heads are placed where the resources would block the path. This functionality was added so that we
    // could later add debug commands which remove the ogre heads and send a path update event
    for(int k = 0; k < 20; ++k)
    {
        for(int j = 0; j < 20; ++j)
        {
            if(p[j][k] == 2)
            {
                Ogre::Entity* entGround = mSceneMgr->createEntity(Ogre::StringConverter::toString(j+k*20), "ogrehead.mesh");
                mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-j*100, -50, -k*100))->attachObject(entGround);
            }
        }
    }

    tiles = new TileSet(p);

    tower = TowerManager::Add(eCannonTower, mSceneMgr, Ogre::Vector3(-1100,0,-1400));
    EnemyManager::Add(ScoutEnemy, 1, 1, 0, 2, "archer.mesh", mSceneMgr, tiles->findShortestPath());
}

//This method reads through our path array and generates the terrain from it. It allows us to customize our terrain
// for testing purposes, however this will run cause your program to run LITERALLY 10 TIMES SLOWER than the above method
void TutorialApplication::generateTerrain()
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
            if(p[j][k] == 0)
            {
                Ogre::Entity* entGround = mSceneMgr->createEntity(Ogre::StringConverter::toString(k+j*20), "ground");
                mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-j*100, -100, -k*100))->attachObject(entGround);
                entGround->setMaterialName("Examples/GrassFloor");
                entGround->setCastShadows(false);
            }
            else
            {
                Ogre::Entity* entGround = mSceneMgr->createEntity(Ogre::StringConverter::toString(k+j*20), "ground");
                mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-j*100, -100, -k*100))->attachObject(entGround);
                entGround->setMaterialName("Examples/Rockwall");
                entGround->setCastShadows(false);
            }
        }
    }
}

bool TutorialApplication::keyPressed( const OIS::KeyEvent &arg )
{
    
    if(arg.key == OIS::KC_1)  // Shorten the path (1)
    {
        p[16][13] = 1;
        mSceneMgr->getEntity(Ogre::StringConverter::toString(16+13*20))->setVisible(false);

        tiles->setTile(16, 13, 1);
        EnemyManager::SetShortestPath(tiles->findShortestPath());
    }
    else if(arg.key == OIS::KC_2)  // Shorten the path (2)
    {
        p[13][7] = 1;
        mSceneMgr->getEntity(Ogre::StringConverter::toString(13+7*20))->setVisible(false);

        tiles->SetTile(13, 7, 1);
        EnemyManager::SetShortestPath(tiles->FindShortestPath());
    }
    else if(arg.key == OIS::KC_3)  // Shorten the path (3)
    {
        p[7][7] = 1;
        p[7][9] = 1;
        p[7][11] = 1;
        mSceneMgr->getEntity(Ogre::StringConverter::toString(7+7*20))->setVisible(false);
        mSceneMgr->getEntity(Ogre::StringConverter::toString(7+9*20))->setVisible(false);
        mSceneMgr->getEntity(Ogre::StringConverter::toString(7+11*20))->setVisible(false);

        tiles->setTile(7, 7, 1);
        tiles->setTile(7, 9, 1);
        tiles->setTile(7, 11, 1);
        EnemyManager::SetShortestPath(tiles->findShortestPath());
    }
    else if (arg.key == OIS::KC_N)  // Kill an enemy
    {
        EnemyManager::Kill();
    }
    else if (arg.key == OIS::KC_U)  // Add a new soldier
    {
        EnemyManager::Add(SoldierEnemy, mSceneMgr, tiles->FindShortestPath());
    }
    else if (arg.key == OIS::KC_I)  // Add a new scout
    {
        EnemyManager::Add(ScoutEnemy, mSceneMgr, tiles->FindShortestPath());
    }
    else if (arg.key == OIS::KC_O)  // Add a new priest
    {
        EnemyManager::Add(PriestEnemy, mSceneMgr, tiles->FindShortestPath());
    }
    else if (arg.key == OIS::KC_P)  // Add a new knight
    {
        EnemyManager::Add(KnightEnemy, mSceneMgr, tiles->FindShortestPath());
    }

    return BaseApplication::keyPressed(arg);
}

bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    EnemyManager::Update(evt, mSceneMgr);
    TowerManager::Update(evt, mSceneMgr);
    
    return BaseApplication::frameRenderingQueued(evt);
}



//#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
//#define WIN32_LEAN_AND_MEAN
//#include "windows.h"
//#endif
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
//    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
//#else
//    int main(int argc, char *argv[])
//#endif
//    {
//        // Create application object
//        TutorialApplication app;
//
//        try {
//            app.go();
//        } catch( Ogre::Exception& e ) {
//#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
//            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
//#else
//            std::cerr << "An exception has occured: " <<
//                e.getFullDescription().c_str() << std::endl;
//#endif
//        }
//
//        return 0;
//    }
//
//#ifdef __cplusplus
//}
//#endif
