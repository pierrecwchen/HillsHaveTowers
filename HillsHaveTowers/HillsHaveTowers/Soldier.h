//-----------------------------------------------------------------------------
// Soldier.h                                                By: Keenan Johnston
// 
// Represents a soldier enemy within the game.  Derived from BaseEnemy with no
// major changes.
//
// All code written by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef SOLDIER_H
#define SOLDIER_H

#include "BaseEnemy.h"
#include "Tile.h"

class Soldier : public BaseEnemy
{
    public:
        friend class EnemyManager;
    
    private:
        // Methods to handle unique deaths from towers
        virtual void DieByFire();
        virtual void PlayEscapeSound();

        // Constructor
        Soldier(std::list<Tile> p_lShortestPath, float p_fXOffset = 1000, float p_fZOffset = 1000);
};

#endif