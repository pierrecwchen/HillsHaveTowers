#include "AdrenalinePower.h"
#include "BaseApplication.h"
#include "SoundManager.h"

AdrenalinePower::AdrenalinePower(Player* p_pPlayer)
    : m_pPlayer(p_pPlayer)
{
    m_sotRequired = SO_NULL;

    m_fTimeUntilReady = 0.0f;
    m_fCoolDown = 40.0f;

    m_fTimeUntilDone = 0.0f;
    m_fActiveLength = 5.0f;
}

void AdrenalinePower::Activate(Selectable* p_sTarget)
{
    SoundManager::Play3DSound("playerSpeedUp.wav", m_pPlayer->GetLocation());

    m_bActive = true;
    m_pPlayer->SetSpeedMultiplier(2.0f);

    m_fTimeUntilDone = m_fActiveLength;
    m_fTimeUntilReady = m_fCoolDown;
}

void AdrenalinePower::Update(Ogre::FrameEvent p_feEvent)
{
    if(m_bActive)
    {
        m_fTimeUntilDone -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

        if(m_fTimeUntilDone <= 0)
        {
            m_pPlayer->SetSpeedMultiplier(1.0f);
            m_fTimeUntilDone = m_fActiveLength;
            m_bActive = false;
        }
    }
    else
    {
        if(m_fTimeUntilReady >= 0)
        {
            m_fTimeUntilReady -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();
        }
    }
}

