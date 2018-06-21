//-----------------------------------------------------------------------------
// Scout.h                                                  By: Keenan Johnston
// 
// Represents a scout enemy within the game.  Derived from BaseEnemy with no
// major changes.
//
// All code written by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef SCOUT_H
#define SCOUT_H

#include "BaseEnemy.h"
#include "Tile.h"

class Scout : public BaseEnemy
{
    public:
        friend class EnemyManager;
    
    private:
        // Methods to handle unique deaths from towers
        virtual void DieByFire();
        virtual void PlayEscapeSound();

        // Constructor
        Scout(std::list<Tile> p_lShortestPath, float p_fXOffset = 1000, float p_fZOffset = 1000);
};

#endif