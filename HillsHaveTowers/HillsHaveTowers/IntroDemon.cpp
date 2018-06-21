#include "IntroDemon.h"

IntroDemon::IntroDemon()
{
    // Store a pointer to the scene m_pCircleNode for convenience
    m_pSceneManager = Ogre::Root::getSingletonPtr()->
                      getSceneManager(BaseApplication::GetSceneManagerName());

    // Get the camera
    m_pCamera = m_pSceneManager->getRootSceneNode()->createChildSceneNode("Cameraman");
    m_pCamera->attachObject(m_pSceneManager->getCamera("PlayerCam"));
    m_vDefaultPos = static_cast<Ogre::Camera*>(m_pCamera->getAttachedObject("PlayerCam"))->getPosition();
    m_vDefaultLook = static_cast<Ogre::Camera*>(m_pCamera->getAttachedObject("PlayerCam"))->getDirection();
    
    // Clone the circle material so we have our own copy to work with
    Ogre::MaterialPtr oldMat = Ogre::MaterialManager::getSingletonPtr()->getByName("RuneCircle");
    m_pCircleMat = oldMat->clone("RuneCircle2", false, "");

    // Make the material fully transparent to begin
    m_pCircleMat->setDiffuse(Ogre::ColourValue(0,0,0,0));
    m_pCircleMat->setSpecular(Ogre::ColourValue(0,0,0,0));
    m_pCircleMat->setAmbient(Ogre::ColourValue(0,0,0,0));
    m_pCircleMat->setSelfIllumination(Ogre::ColourValue(0,0,0,0));

    // Create a simple plane and apply the circle material to it
    m_pCircle = m_pSceneManager->createManualObject("Rune Circle");
    m_pCircle->begin("RuneCircle2", Ogre::RenderOperation::OT_TRIANGLE_LIST);

    m_pCircle->position( 10.0, 0.0,  10.0);
    m_pCircle->textureCoord(1.0, 1.0);
    m_pCircle->position( 10.0, 0.0, -10.0);
    m_pCircle->textureCoord(1.0, 0.0);
    m_pCircle->position(-10.0, 0.0, -10.0);
    m_pCircle->textureCoord(0.0, 0.0);

    m_pCircle->position(-10.0, 0.0, -10.0);
    m_pCircle->textureCoord(0.0, 0.0);
    m_pCircle->position(-10.0, 0.0,  10.0);
    m_pCircle->textureCoord(0.0, 1.0);
    m_pCircle->position( 10.0, 0.0,  10.0);
    m_pCircle->textureCoord(1.0, 1.0);

    m_pCircle->end();
    m_pCircle->setCastShadows(false);

    // Attach the plane to a scene m_pCircleNode and move it to the proper location
    m_pCircleNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode("RuneNode");
    m_pCircleNode->attachObject(m_pCircle);
    m_pCircleNode->translate(TileSet::GetStartingTile()->GetLocation());
    m_pCircleNode->translate(0, 2, 0);
    
    // Create the overlay used for the black flash
    m_pBlackOverlay = Ogre::OverlayManager::getSingleton().getByName("Overlays/Black");
    m_pOverlayMat = Ogre::MaterialManager::getSingletonPtr()->getByName("Black1");

    // Set the initial values for the update method
    m_fTimer = 8.0f;
    m_fFade = 0.0f;
    m_fStage = -1;
    m_bPlaying = true;
    m_bAttackPlaying = false;
    m_pBlackOverlay->show();
}

IntroDemon::~IntroDemon()
{
    delete m_pCultist1;
    delete m_pCultist2;
    delete m_pCultist3;
    delete m_pCultist4;

    m_pCircleMat->unload();
    m_pDemonMat->unload();

    m_pCircleNode->getParentSceneNode()->removeAndDestroyChild("RuneNode");
    //delete m_pCircle;

    m_pSceneManager->destroyParticleSystem("demon_fire1");
    m_pSceneManager->destroyParticleSystem("demon_fire2");
    m_pSceneManager->destroyParticleSystem("demon_fire3");
    m_pSceneManager->destroyParticleSystem("demon_fire4");
    //delete m_pDemonFire;

    //delete m_pBlackOverlay;

    m_pSceneManager = NULL;
}

void IntroDemon::Update(const Ogre::FrameEvent& p_feEvent)
{
    // Decrement timer        
    m_fTimer -= p_feEvent.timeSinceLastFrame;

    // Rotate the circle
    m_pCircleNode->yaw(Ogre::Degree(p_feEvent.timeSinceLastFrame*15));

    if (m_fStage > 0)
    {
        m_pCultist1->Update(p_feEvent);
        m_pCultist2->Update(p_feEvent);
        m_pCultist3->Update(p_feEvent);
        m_pCultist4->Update(p_feEvent);
    }

    //-----------------------------------------------------------------
    // Spawn cultists and move them to the first tile
    //
    //-----------------------------------------------------------------
    if (m_fStage == -1)
    {
        SoundManager::StopMusic();
        if (m_fFade <= 1)
        {
            m_pOverlayMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, m_fFade);
            m_fFade += p_feEvent.timeSinceLastFrame/2;
        }
        else if (m_fFade > 1)
        {
            SoundManager::Play2DSound("cutsceneStart.wav", 0.0f, 0.5f);
            m_fStage++;
        }
    }
    else if (m_fStage == 0)
    {        
        // Place the camera
        static_cast<Ogre::Camera*>(m_pCamera->getAttachedObject("PlayerCam"))->setPosition(0,0,0);

        m_pCamera->setPosition(TileSet::GetStartingTile()->GetLocation().x, 
                               TileSet::GetStartingTile()->GetLocation().y + 75,
                               TileSet::GetStartingTile()->GetLocation().z);

        Ogre::Vector3 cam = static_cast<Ogre::Camera*>(m_pCamera->getAttachedObject("PlayerCam"))->getPosition();

        // Create the cultists, size and position them properly and set their material
        m_pCultist1 = new Model("MageMale.mesh");
        m_pCultist2 = new Model("MageMale.mesh");
        m_pCultist3 = new Model("MageMale.mesh");
        m_pCultist4 = new Model("MageMale.mesh");

        m_pCultist1->SetAnimationSpeed(0.8f);
        m_pCultist2->SetAnimationSpeed(0.8f);
        m_pCultist3->SetAnimationSpeed(0.8f);
        m_pCultist4->SetAnimationSpeed(0.8f);

        m_pCultist1->UseTerrainHeight(true);
        m_pCultist2->UseTerrainHeight(true);
        m_pCultist3->UseTerrainHeight(true);
        m_pCultist4->UseTerrainHeight(true);

        m_pCultist1->SetHeightAdjustment(6.0f);
        m_pCultist2->SetHeightAdjustment(6.0f);
        m_pCultist3->SetHeightAdjustment(6.0f);
        m_pCultist4->SetHeightAdjustment(6.0f);

        m_pCultist1->Scale(Ogre::Vector3(0.4f));
        m_pCultist2->Scale(Ogre::Vector3(0.4f));
        m_pCultist3->Scale(Ogre::Vector3(0.4f));
        m_pCultist4->Scale(Ogre::Vector3(0.4f));

        m_pCultist1->GetEntity()->setMaterialName("Cultist");
        m_pCultist2->GetEntity()->setMaterialName("Cultist");
        m_pCultist3->GetEntity()->setMaterialName("Cultist");
        m_pCultist4->GetEntity()->setMaterialName("Cultist");

        m_pCultist1->PlayAnimation("Walk", true);
        m_pCultist2->PlayAnimation("Walk", true);
        m_pCultist3->PlayAnimation("Walk", true);
        m_pCultist4->PlayAnimation("Walk", true);

        Tile* pStart = TileSet::GetStartingTile();

        m_pCultist1->Translate(Ogre::Vector3(pStart->GetLocation().x - Tile::GetDefaultSize()/1.6, 
                                                pStart->GetLocation().y + 12.0f, 
                                                pStart->GetLocation().z - Tile::GetDefaultSize()/1.6));
        m_pCultist2->Translate(Ogre::Vector3(pStart->GetLocation().x + Tile::GetDefaultSize()/1.6, 
                                                pStart->GetLocation().y + 12.0f, 
                                                pStart->GetLocation().z - Tile::GetDefaultSize()/1.6));
        m_pCultist3->Translate(Ogre::Vector3(pStart->GetLocation().x + Tile::GetDefaultSize()/1.86, 
                                                pStart->GetLocation().y + 12.0f, 
                                                pStart->GetLocation().z + Tile::GetDefaultSize()/1.6));
        m_pCultist4->Translate(Ogre::Vector3(pStart->GetLocation().x - Tile::GetDefaultSize()/1.6, 
                                                pStart->GetLocation().y + 12.0f, 
                                                pStart->GetLocation().z + Tile::GetDefaultSize()/1.6));

        // Position properly based on starting tile location
        if (pStart->GetGridX() == 0)
        {
            m_pCultist1->Translate(Ogre::Vector3(-Tile::GetDefaultSize() * 3.5, 0, 0));
            m_pCultist2->Translate(Ogre::Vector3(-Tile::GetDefaultSize() * 3.5, 0, 0));
            m_pCultist3->Translate(Ogre::Vector3(-Tile::GetDefaultSize() * 3.5, 0, 0));
            m_pCultist4->Translate(Ogre::Vector3(-Tile::GetDefaultSize() * 3.5, 0, 0));
            m_pCamera->setPosition(m_pCamera->getPosition() + Ogre::Vector3(Tile::GetDefaultSize(), 0, 0));
            m_vDirection = Ogre::Vector3(-1,0,0);
        }
        else if (pStart->GetGridY() == 0)
        {
            m_pCultist1->Translate(Ogre::Vector3(0, 0, -Tile::GetDefaultSize() * 3.5));
            m_pCultist2->Translate(Ogre::Vector3(0, 0, -Tile::GetDefaultSize() * 3.5));
            m_pCultist3->Translate(Ogre::Vector3(0, 0, -Tile::GetDefaultSize() * 3.5));
            m_pCultist4->Translate(Ogre::Vector3(0, 0, -Tile::GetDefaultSize() * 3.5));
            m_pCamera->setPosition(m_pCamera->getPosition() + Ogre::Vector3(0, 0, Tile::GetDefaultSize()));
            m_vDirection = Ogre::Vector3(0,0,-1);
        }
        else if (pStart->GetGridX() == TileSet::GetWidth() - 1)
        {
            m_pCultist1->Translate(Ogre::Vector3(Tile::GetDefaultSize() * 3.5, 0, 0));
            m_pCultist2->Translate(Ogre::Vector3(Tile::GetDefaultSize() * 3.5, 0, 0));
            m_pCultist3->Translate(Ogre::Vector3(Tile::GetDefaultSize() * 3.5, 0, 0));
            m_pCultist4->Translate(Ogre::Vector3(Tile::GetDefaultSize() * 3.5, 0, 0));
            m_pCamera->setPosition(m_pCamera->getPosition() + Ogre::Vector3(-Tile::GetDefaultSize(), 0, 0));
            m_vDirection = Ogre::Vector3(1,0,0);
        }
        else if (pStart->GetGridY() == TileSet::GetLength() - 1)
        {
            m_pCultist1->Translate(Ogre::Vector3(0, 0, Tile::GetDefaultSize() * 3.5));
            m_pCultist2->Translate(Ogre::Vector3(0, 0, Tile::GetDefaultSize() * 3.5));
            m_pCultist3->Translate(Ogre::Vector3(0, 0, Tile::GetDefaultSize() * 3.5));
            m_pCultist4->Translate(Ogre::Vector3(0, 0, Tile::GetDefaultSize() * 3.5));
            m_pCamera->setPosition(m_pCamera->getPosition() + Ogre::Vector3(0, 0, -Tile::GetDefaultSize()));
            m_vDirection = Ogre::Vector3(0,0,-1);
        }

        // Face them
        m_pCultist1->FaceTo(Ogre::Vector3(pStart->GetLocation().x - Tile::GetDefaultSize()/1.6, 
                                            pStart->GetLocation().y + 12.0f, 
                                            pStart->GetLocation().z - Tile::GetDefaultSize()/1.6));
        m_pCultist2->FaceTo(Ogre::Vector3(pStart->GetLocation().x + Tile::GetDefaultSize()/1.6, 
                                            pStart->GetLocation().y + 12.0f, 
                                            pStart->GetLocation().z - Tile::GetDefaultSize()/1.6));
        m_pCultist3->FaceTo(Ogre::Vector3(pStart->GetLocation().x + Tile::GetDefaultSize()/1.6, 
                                            pStart->GetLocation().y + 12.0f, 
                                            pStart->GetLocation().z + Tile::GetDefaultSize()/1.6));
        m_pCultist4->FaceTo(Ogre::Vector3(pStart->GetLocation().x - Tile::GetDefaultSize()/1.6, 
                                            pStart->GetLocation().y + 12.0f, 
                                            pStart->GetLocation().z + Tile::GetDefaultSize()/1.6));

        // Move the cultists forward
        m_pCultist1->TranslateToWithSpeed(Tile::GetDefaultSize()/3, Ogre::Vector3(pStart->GetLocation().x - Tile::GetDefaultSize()/1.6, 
                                                                                    pStart->GetLocation().y + 12.0f, 
                                                                                    pStart->GetLocation().z - Tile::GetDefaultSize()/1.6));
        m_pCultist2->TranslateToWithSpeed(Tile::GetDefaultSize()/3, Ogre::Vector3(pStart->GetLocation().x + Tile::GetDefaultSize()/1.6, 
                                                                                    pStart->GetLocation().y + 12.0f, 
                                                                                    pStart->GetLocation().z - Tile::GetDefaultSize()/1.6));
        m_pCultist3->TranslateToWithSpeed(Tile::GetDefaultSize()/3, Ogre::Vector3(pStart->GetLocation().x + Tile::GetDefaultSize()/1.6, 
                                                                                    pStart->GetLocation().y + 12.0f, 
                                                                                    pStart->GetLocation().z + Tile::GetDefaultSize()/1.6));
        m_pCultist4->TranslateToWithSpeed(Tile::GetDefaultSize()/3, Ogre::Vector3(pStart->GetLocation().x - Tile::GetDefaultSize()/1.6, 
                                                                                    pStart->GetLocation().y + 12.0f, 
                                                                                    pStart->GetLocation().z + Tile::GetDefaultSize()/1.6));

        static_cast<Ogre::Camera*>(m_pCamera->getAttachedObject("PlayerCam"))->setAutoTracking(true, m_pCultist1->GetSceneNode(), Ogre::Vector3(Tile::GetDefaultSize()/1.6, 0, Tile::GetDefaultSize()/1.6));

        m_fStage++;
    }
    //-----------------------------------------------------------------
    // Spawn cultists and move them to the first tile
    //
    //-----------------------------------------------------------------
    if (m_fStage == 1)
    {
        if (m_fFade >= 0)
        {
            m_pOverlayMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, m_fFade);
            m_fFade -= p_feEvent.timeSinceLastFrame/2;
        }
        else if (m_fFade <= 0)
        {
            m_pBlackOverlay->hide();
            m_fFade = 1.0f;
        }
        
        m_pCamera->setPosition(m_pCamera->getPosition() - Ogre::Vector3(0, p_feEvent.timeSinceLastFrame, 0));
        
        if(!m_pCultist1->IsTranslating())
        {
            m_iChantingChannel = SoundManager::Play3DLoopedSound("chanting.wav", TileSet::GetStartingTile()->GetLocation(), 0.5f);
            m_pCultist1->SetAnimationSpeed(0.2f);
            m_pCultist1->PlayAnimation("Hit", true);
            m_pCultist1->FaceTo(TileSet::GetStartingTile()->GetLocation());
            //m_pCultist1->RotateOverTime(1, m_pCultist1->GetAngleTo(TileSet::GetStartingTile()->GetLocation()));

            m_pCultist2->SetAnimationSpeed(0.2f);
            m_pCultist2->PlayAnimation("Hit", true);
            m_pCultist2->FaceTo(TileSet::GetStartingTile()->GetLocation());
            //m_pCultist2->RotateOverTime(1, m_pCultist2->GetAngleTo(TileSet::GetStartingTile()->GetLocation()));

            m_pCultist3->SetAnimationSpeed(0.2f);
            m_pCultist3->PlayAnimation("Hit", true);
            m_pCultist3->FaceTo(TileSet::GetStartingTile()->GetLocation());
            //m_pCultist3->RotateOverTime(1, m_pCultist3->GetAngleTo(TileSet::GetStartingTile()->GetLocation()));

            m_pCultist4->SetAnimationSpeed(0.2f);
            m_pCultist4->PlayAnimation("Hit", true);
            m_pCultist4->FaceTo(TileSet::GetStartingTile()->GetLocation());
            //m_pCultist4->RotateOverTime(1, m_pCultist4->GetAngleTo(TileSet::GetStartingTile()->GetLocation()));

            m_fTimer = 2.0f;
            m_fStage++;
        }               
    }
    //-----------------------------------------------------------------
    // For the first stage of the cutscene, fade in the rune circle
    // from invisible to full opacity.
    //
    //-----------------------------------------------------------------
    if (m_fStage == 2)
    {
        if (m_pCamera->getPosition().y <= 65)
        {
            float fDistance = (m_pCamera->getPosition().y - 60.0f) * 5.0f;
            m_pCamera->setPosition(m_pCamera->getPosition() - Ogre::Vector3(0, p_feEvent.timeSinceLastFrame / (26.0f - fDistance), 0));
        }
        if (m_pCamera->getPosition().y >= 65)    
            m_pCamera->setPosition(m_pCamera->getPosition() - Ogre::Vector3(0, p_feEvent.timeSinceLastFrame, 0));
        
        if (m_pCircleMat->getTechnique(0)->getPass(0)->getEmissive().a <= 1 && m_fTimer <= 0.0f)
        {
            m_pCircleMat->getTechnique(0)->getPass(0)->setEmissive(m_pCircleMat->getTechnique(0)->getPass(0)->getEmissive() +
                                                                   Ogre::ColourValue(p_feEvent.timeSinceLastFrame/8,
                                                                                     p_feEvent.timeSinceLastFrame/8,
                                                                                     p_feEvent.timeSinceLastFrame/8,
                                                                                     p_feEvent.timeSinceLastFrame/8));
        }
        else if (m_pCircleMat->getTechnique(0)->getPass(0)->getEmissive().a > 1)
            m_fStage++;
    }
    //-----------------------------------------------------------------
    // For the second stage of the cutscene, start the small fire in
    // the middle of the circle and wait 5 seconds before moving on.
    //-----------------------------------------------------------------
    else if (m_fStage == 3)
    {
        if (m_pCamera->getPosition().y <= 65)
        {
            float fDistance = (m_pCamera->getPosition().y - 60.0f) * 5.0f;
            m_pCamera->setPosition(m_pCamera->getPosition() - Ogre::Vector3(0, p_feEvent.timeSinceLastFrame / (26.0f - fDistance), 0));
        }
        if (m_pCamera->getPosition().y >= 65)    
            m_pCamera->setPosition(m_pCamera->getPosition() - Ogre::Vector3(0, p_feEvent.timeSinceLastFrame, 0));
        
        m_iFireChannel = SoundManager::Play3DLoopedSound("torchIdle.wav", TileSet::GetStartingTile()->GetLocation(), 0.4f);
        m_pDemonFire = m_pSceneManager->createParticleSystem("demon_fire1", "Bonfire");
        m_fTimer = 5.0f;
        m_pCircleNode->attachObject(m_pDemonFire);
        m_fStage++;
    }

    else if (m_fStage == 4)
    {
        if (m_pCamera->getPosition().y <= 65)
        {
            float fDistance = (m_pCamera->getPosition().y - 60.0f) * 5.0f;
            m_pCamera->setPosition(m_pCamera->getPosition() - Ogre::Vector3(0, p_feEvent.timeSinceLastFrame / (26.0f - fDistance), 0));
        }
        if (m_pCamera->getPosition().y >= 65)    
            m_pCamera->setPosition(m_pCamera->getPosition() - Ogre::Vector3(0, p_feEvent.timeSinceLastFrame, 0));
        
        if (m_fTimer <= 0)
        {
            SoundManager::StopSoundAtChannel(m_iFireChannel);
            m_iFireChannel = SoundManager::Play3DLoopedSound("torchIdle.wav", TileSet::GetStartingTile()->GetLocation(), 0.8f);
            m_pDemonFire->setEmitting(false);
            m_pDemonFire = m_pSceneManager->createParticleSystem("demon_fire2", "DemonFireMedium");
            m_fTimer = 5.0f;
            m_pCircleNode->attachObject(m_pDemonFire);
            m_fStage++;
        }
    }
    //-----------------------------------------------------------------
    // For the third stage of the cutscene, stop the small fire and
    // start the larger one.  Wait another 5 seconds before moving on.
    //-----------------------------------------------------------------
    else if (m_fStage == 5)
    {
        m_pCamera->setPosition(m_pCamera->getPosition() - Ogre::Vector3(m_vDirection.x*p_feEvent.timeSinceLastFrame*1.8,
                                                                        -p_feEvent.timeSinceLastFrame*1.8,
                                                                        m_vDirection.z*p_feEvent.timeSinceLastFrame*1.8));
        static_cast<Ogre::Camera*>(m_pCamera->getAttachedObject("PlayerCam"))->
          setAutoTracking(true, m_pCultist1->GetSceneNode(), static_cast<Ogre::Camera*>(m_pCamera->getAttachedObject("PlayerCam"))->getAutoTrackOffset() +
          Ogre::Vector3(0, p_feEvent.timeSinceLastFrame*1.4, 0));
        
        if (m_fTimer <= 0)
        {
            SoundManager::StopSoundAtChannel(m_iFireChannel);
            m_iFireChannel = SoundManager::Play3DLoopedSound("bossSummonIntenseFire.wav", TileSet::GetStartingTile()->GetLocation(), 0.5f);
            m_pDemonFire->setEmitting(false);
            m_pDemonFire = m_pSceneManager->createParticleSystem("demon_fire3", "DemonFireLarge");
            m_fTimer = 5.0f;
            m_pCircleNode->attachObject(m_pDemonFire);
            m_fStage++;
        }
    }
    //-----------------------------------------------------------------
    // For the fourth stage of the cutscene, stop the previous fire and
    // start the very large one.  Wait 1.5 seconds before moving on.
    //-----------------------------------------------------------------
    else if (m_fStage == 6)
    {
        m_pCamera->setPosition(m_pCamera->getPosition() - Ogre::Vector3(m_vDirection.x*p_feEvent.timeSinceLastFrame*1.8,
                                                                        -p_feEvent.timeSinceLastFrame*1.8,
                                                                        m_vDirection.z*p_feEvent.timeSinceLastFrame*1.8));
        static_cast<Ogre::Camera*>(m_pCamera->getAttachedObject("PlayerCam"))->
          setAutoTracking(true, m_pCultist1->GetSceneNode(), static_cast<Ogre::Camera*>(m_pCamera->getAttachedObject("PlayerCam"))->getAutoTrackOffset() +
          Ogre::Vector3(0, p_feEvent.timeSinceLastFrame*1.4, 0));
        
        if (m_fTimer <= 0)
        {
            SoundManager::StopSoundAtChannel(m_iFireChannel);
            m_iFireChannel = SoundManager::Play3DLoopedSound("bossSummonIntenseFire.wav", TileSet::GetStartingTile()->GetLocation(), 0.8f);
            m_pDemonFire->setEmitting(false);
            m_pDemonFire = m_pSceneManager->createParticleSystem("demon_fire4", "DemonFireHuge");
            m_pCircleNode->attachObject(m_pDemonFire);
            m_fTimer = 1.5f;
            m_fStage++;
        }
    }
    //-----------------------------------------------------------------
    // For the fifth stage of the cutscene, create the demon and make
    // him invisible for now.
    //-----------------------------------------------------------------
    else if (m_fTimer <= 0 && m_fStage == 7)
    {  
        m_pDemon = static_cast<Demon*>(EnemyManager::Instance()->Add(ET_DEMON));
        m_pDemon->GetModel()->PlayAnimation("Idle", true);
        m_pDemon->GetModel()->TranslateTo(Ogre::Vector3(m_pCircleNode->getPosition().x, 
                                                        m_pDemon->GetModel()->GetLocation().y, 
                                                        m_pCircleNode->getPosition().z));

        m_pDemonMat = Ogre::MaterialManager::getSingleton().getByName("demon");
        m_pDemonMat->setAmbient(Ogre::ColourValue(0,0,0,0));
        m_pDemonMat->setDiffuse(Ogre::ColourValue(0,0,0,0));
        m_pDemonMat->setSpecular(Ogre::ColourValue(0,0,0,0));
        m_pDemonMat->setSelfIllumination(Ogre::ColourValue(0,0,0,0));

        m_bAttackPlaying = false;
        m_fRotation = 90.0f;
        m_fStage++;
    }
    //-----------------------------------------------------------------
    // For the sixth stage of the cutscene, start fading in the demon
    // to 50% opacity over 8 seconds.  Play his attack animation after
    // the fade, then flash the screen black in the middle of it and
    // set him to full opacity and stop the large fire from emitting.
    //-----------------------------------------------------------------
    else if (m_fStage == 8)
    {
        // Rotate the camera around the circle
        if (m_fRotation <= 450.0f)
        {
            m_pCamera->setPosition(static_cast<Ogre::Camera*>(m_pCamera->getAttachedObject("PlayerCam"))->getAutoTrackOffset() +
                                   m_pCultist1->GetLocation() + 
                                   Ogre::Vector3((-sin(m_fRotation*3.14159/180) * 30.977f), 51.737f, (-cos(m_fRotation*3.14159/180) * 30.977f)));
            m_fRotation += p_feEvent.timeSinceLastFrame*20;
        }

        if (m_pDemonMat->getTechnique(0)->getPass(0)->getAmbient().a <= 0.5)
        {
            Ogre::ColourValue phase = Ogre::ColourValue(0, 0, 0, p_feEvent.timeSinceLastFrame/40);

            m_pDemonMat->setAmbient(m_pDemonMat->getTechnique(0)->getPass(0)->getAmbient()+phase);
            m_pDemonMat->setDiffuse(m_pDemonMat->getTechnique(0)->getPass(0)->getDiffuse()+phase);
            m_pDemonMat->setSpecular(m_pDemonMat->getTechnique(0)->getPass(0)->getSpecular()+phase);
            m_pDemonMat->setSelfIllumination(m_pDemonMat->getTechnique(0)->getPass(0)->getEmissive()+phase);
        }

        if (m_fRotation >= 450.0f)
        {
            SoundManager::Play3DSound("bossLaugh.wav", m_pDemon->GetLocation());
            m_pDemon->GetModel()->PlayAnimation("Attack", false);
            m_bAttackPlaying = true;
            m_fTimer = 2.200f;
            m_fStage++;
        }
    }
    else if (m_fStage == 9)
    {
        // Zoom out slightly
        m_pCamera->setPosition(m_pCamera->getPosition() - Ogre::Vector3(m_vDirection.x*p_feEvent.timeSinceLastFrame*1.2,
                                                                        -p_feEvent.timeSinceLastFrame*1.2,
                                                                        m_vDirection.z*p_feEvent.timeSinceLastFrame*1.2));
        
        if (m_fTimer <= 0)
        {
            m_pBlackOverlay->hide();
            m_fTimer = m_pDemon->GetModel()->GetAnimationState()->getLength() - m_pDemon->GetModel()->GetAnimationState()->getTimePosition();
            m_fStage++;
        }
        else if (m_fTimer <= 0.1)
        {
            SoundManager::StopSoundAtChannel(m_iFireChannel);
            SoundManager::Play3DSound("bossSummonBoom.wav", TileSet::GetStartingTile()->GetLocation());
            m_pBlackOverlay->show();
            m_pDemonMat->setSceneBlending(Ogre::SceneBlendFactor::SBF_ONE, Ogre::SceneBlendFactor::SBF_ZERO);
            m_pDemonMat->setDepthWriteEnabled(true);
            m_pDemonMat->setAmbient(Ogre::ColourValue(1,1,1,1));
            m_pDemonMat->setDiffuse(Ogre::ColourValue(1,1,1,1));
            m_pDemonMat->setSpecular(Ogre::ColourValue(1,1,1,1));

            m_pCultist1->SetAnimationSpeed(1.0f);
            m_pCultist2->SetAnimationSpeed(1.0f);
            m_pCultist3->SetAnimationSpeed(1.0f);
            m_pCultist4->SetAnimationSpeed(1.0f);

            SoundManager::StopSoundAtChannel(m_iChantingChannel);
            m_pCultist1->PlayAnimation("Die", false);
            m_pCultist2->PlayAnimation("Die", false);
            m_pCultist3->PlayAnimation("Die", false);
            m_pCultist4->PlayAnimation("Die", false);

            m_pDemonFire->setEmitting(false);
        }
    }
    //-----------------------------------------------------------------
    // For the seventh stage of the cutscene, fade out the rune circle
    // and destroy it.
    //-----------------------------------------------------------------
    else if (m_fStage == 10)
    {
        // Start fading the circle back out again
        m_pCircleMat->getTechnique(0)->getPass(0)->setEmissive(m_pCircleMat->getTechnique(0)->getPass(0)->getEmissive() -
                                                               Ogre::ColourValue(p_feEvent.timeSinceLastFrame/5,
                                                                                 p_feEvent.timeSinceLastFrame/5,
                                                                                 p_feEvent.timeSinceLastFrame/5,
                                                                                 p_feEvent.timeSinceLastFrame/5));

        if (m_pCircleMat->getTechnique(0)->getPass(0)->getEmissive().a <= 0.0)
        {
            m_pCultist1->GetSceneNode()->setVisible(false);
            m_pCultist2->GetSceneNode()->setVisible(false);
            m_pCultist3->GetSceneNode()->setVisible(false);
            m_pCultist4->GetSceneNode()->setVisible(false);
        }
        
        // Play the idle animation after the attack animation has finished
        //if (m_pDemon->GetModel()->HasAnimationEnded() && m_bAttackPlaying)
        if (m_fTimer <= 0 && m_bAttackPlaying)
        {
            m_pDemon->GetModel()->PlayAnimation("Idle_2", false);
            m_fTimer = 1.6667 * 2;
            m_bAttackPlaying = false;
        }
        // After the idle animation has finished, go into walk state and finish
        // the cutscene
        //else if (m_pDemon->GetModel()->HasAnimationEnded() && !m_bAttackPlaying)
        else if (m_fTimer <= 0 && !m_bAttackPlaying && m_pCircleMat->getTechnique(0)->getPass(0)->getEmissive().a <= 0.0)
        {
            m_pDemon->GetModel()->PlayAnimation("Idle", true);
            m_fFade = 0.0f;
            m_pBlackOverlay->show();
            m_fStage++;
        }
    }
    else if (m_fStage == 11)
    {
        if (m_fFade <= 1)
        {
            m_pOverlayMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, m_fFade);
            m_fFade += p_feEvent.timeSinceLastFrame;
        }
        else if (m_fFade > 1)
        {
            // Move the camera back to gameplay view
            m_pCamera->detachAllObjects();
            m_pSceneManager->getCamera("PlayerCam")->setAutoTracking(false);
            m_pSceneManager->getCamera("PlayerCam")->setPosition(m_vDefaultPos);
            m_pSceneManager->getCamera("PlayerCam")->setDirection(m_vDefaultLook);

            SoundManager::PlayMusic("bossMusic.wav");

            // Start the boss moving
            m_pDemon->SetState(WalkState);
            m_pDemon->GetModel()->PlayAnimation("Walk", true);

            m_fStage++;
        }
    }
    else if (m_fStage == 12)
    {
        if (m_fFade >= 0)
        {
            m_pOverlayMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, m_fFade);
            m_fFade -= p_feEvent.timeSinceLastFrame/2;
        }
        else if (m_fFade <= 0)
        {
            m_pBlackOverlay->hide();
            m_fFade = 1.0f;
            m_fStage++;
        }        
    }
    else if (m_fStage == 13)
    {        
        m_bPlaying = false;
    }

    SoundManager::PositionListener(m_pCamera->getPosition(),
                                   m_pSceneManager->getCamera("PlayerCam")->getDirection());
}