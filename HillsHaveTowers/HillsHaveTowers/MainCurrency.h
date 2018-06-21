//-----------------------------------------------------------------------------
// MainCurrency.h                                           By: Keenan Johnston
// 
// Represents the player's current resources within the game.  Can be updated
// with currency objects or through individual fields and communicates with the
// HUD to keep its values updated for the player.  This class is also static
// since only one instance of it will ever be needed.
//
// All code by Keenan unless otherwise noted.
//-----------------------------------------------------------------------------

#ifndef MAINCURRENCY_H
#define MAINCURRENCY_H

#include "InGameHUD.h"
#include "Currency.h"

class MainCurrency
{
    public:
        // General setter, for start of a round and debug
        static void SetCurrency(int p_iGold, int p_iWood, int p_iStone, int p_iIron);

        // Check if enough resources can be subtracted from the one passed in
        // that none go below zero
        static bool CanAfford(Currency p_rResource);

        // Add or subtract currency from this one
        static void Add(Currency p_rResource);
        static void Subtract(Currency p_rResource);

        // Add a percentage of the given resources back
        static void Refund(Currency p_rResource);

        // Add a given amount of one type of resource
        static void AddGold(int p_iGold);
        static void AddWood(int p_iWood);
        static void AddStone(int p_iStone);
        static void AddIron(int p_iIron);

        // Get individual resource values
        static inline int GetGold() { return m_iGold; }
        static inline int GetWood() { return m_iWood; }
        static inline int GetStone() { return m_iStone; }
        static inline int GetIron() { return m_iIron; }

    private:
        // Individual resource types
        static int m_iGold, m_iWood, m_iStone, m_iIron;
        static float m_fPercentage;
};

#endif