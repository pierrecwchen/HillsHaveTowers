/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.h
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
#ifndef __TutorialApplication_h_
#define __TutorialApplication_h_

#include "stdafx.h"
#include "BaseApplication.h"

#include "EnemyManager.h"
#include "TowerManager.h"
#include "TileSet.h"

class TutorialApplication : public BaseApplication
{
public:
    TutorialApplication(void);
    virtual ~TutorialApplication(void);

    bool frameRenderingQueued(const Ogre::FrameEvent& evt);

protected:
    virtual void createScene(void);
    virtual bool keyPressed( const OIS::KeyEvent &arg );

private:
    void generateHardLevel();
    void generateTerrain();

    TileSet* tiles;
    BaseEnemy* enemy;
    BaseTower* tower;

    int** p; //Gotta make this short for readability
};

#endif // #ifndef __TutorialApplication_h_
