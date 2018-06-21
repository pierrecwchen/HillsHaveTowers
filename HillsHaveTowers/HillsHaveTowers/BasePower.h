#ifndef BASE_POWER_H
#define BASE_POWER_H

#include "Selectable.h"

class BasePower
{
public:
    BasePower() {}

    SelectableObjType TypeRequired() { return m_sotRequired; }

    float GetCoolDown() { return m_fCoolDown; }
    float GetTimeUntilReady() { return m_fTimeUntilReady; }
    bool IsActive() { return m_bActive; }
    bool IsReady() { return m_fTimeUntilReady <= 0; }

    virtual void Activate(Selectable* p_sTarget = 0) = 0;
    virtual void Update(Ogre::FrameEvent p_feEvent) = 0;

protected:
    float m_fCoolDown, m_fTimeUntilReady, m_fActiveLength, m_fTimeUntilDone;

    SelectableObjType m_sotRequired;
    bool m_bActive;
};

#endif

