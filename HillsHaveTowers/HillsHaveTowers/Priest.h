//-----------------------------------------------------------------------------
// Priest.h                                                 By: Keenan Johnston
// 
// Represents a priest enemy within the game.  Derived from BaseEnemy and
// overrides the update method to include the healing ability of this enemy.
//
// All code written by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef PRIEST_H
#define PRIEST_H

#include "BaseEnemy.h"
#include "Tile.h"

class Priest : public BaseEnemy
{
    public:
        friend class EnemyManager;
        
        // Overridden update method
        void Update(const Ogre::FrameEvent& p_feEvent);
    
    private:
        // Time between heal events
        float m_fHealDelay;

        // Methods to handle unique deaths from towers
        virtual void DieByFire();
        virtual void PlayEscapeSound() {};

        // Constructor
        Priest(std::list<Tile> p_lShortestPath, float p_fXOffset = 1000, float p_fZOffset = 1000);
};

#endif