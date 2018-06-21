#ifndef METEOR_POWER_H
#define METEOR_POWER_H

#include "BasePower.h"
#include "Model.h"

class MeteorPower : public BasePower
{
public:
    MeteorPower();

    virtual void Activate(Selectable* p_sTarget = 0);
    virtual void Update(Ogre::FrameEvent p_feEvent);

private:
    std::vector<Model*> m_vMeteors;
    std::vector<bool> m_vPlayedWooshSound;

    void PlayHitSound(Ogre::Vector3 p_v3Location);
};

#endif