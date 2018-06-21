#ifndef INTRODEMON_H
#define INTRODEMON_H

#include "Model.h"
#include "EnemyManager.h"
#include "TileSet.h"
#include "SoundManager.h"

class IntroDemon
{
    public:
        IntroDemon();
        ~IntroDemon();

        bool Playing() { return m_bPlaying; }

        void Update(const Ogre::FrameEvent& p_feEvent);

    private:
        float m_fTimer, m_fStage, m_fRotation, m_fFade;
        bool m_bAttackPlaying, m_bPlaying;
        Ogre::Vector3 m_vDefaultPos, m_vDefaultLook;
        Ogre::Vector3 m_vDirection;

        int m_iChantingChannel, m_iFireChannel;

        Ogre::SceneManager* m_pSceneManager;
        Ogre::MaterialPtr m_pCircleMat, m_pDemonMat, m_pOverlayMat;
        Ogre::ManualObject* m_pCircle;
        Ogre::SceneNode* m_pCircleNode, *m_pCamera;
        Ogre::Overlay* m_pBlackOverlay;
        Ogre::ParticleSystem* m_pDemonFire;
        Demon* m_pDemon;
        Model* m_pCultist1, *m_pCultist2, *m_pCultist3, *m_pCultist4;
};

#endif