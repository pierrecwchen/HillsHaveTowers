#ifndef ADREN_POWER_H
#define ADREN_POWER_H

#include "BasePower.h"
#include "Player.h"

class AdrenalinePower : public BasePower
{
public:
    AdrenalinePower(Player* p_pPlayer);

    virtual void Activate(Selectable* p_sTarget = 0);
    virtual void Update(Ogre::FrameEvent p_feEvent);

private:
    Player* m_pPlayer;
};

#endif
