//-----------------------------------------------------------------------------
// Currency.h                                               By: Keenan Johnston
// 
// Simple data class representing the four currencies within the game in one
// object.  Used to represent costs and awards which are added/subtracted from
// the main currency.
//-----------------------------------------------------------------------------

#ifndef CURRENCY_H
#define CURRENCY_H

class Currency
{
    public:
        // Constructors
        Currency() {}
        Currency(int p_iGold, int p_iWood, int p_iStone, int p_iIron, bool p_bMain = false) :
                 m_iGold(p_iGold), m_iWood(p_iWood), m_iStone(p_iStone), m_iIron(p_iIron) {}

        // Add a given amount of one type of resource
        inline void AddGold(int p_iGold) { m_iGold += p_iGold; }
        inline void AddWood(int p_iWood) { m_iWood += p_iWood; }
        inline void AddStone(int p_iStone) { m_iStone += p_iStone; }
        inline void AddIron(int p_iIron) { m_iIron += p_iIron; }

        // Get individual resource values
        inline int GetGold() { return m_iGold; }
        inline int GetWood() { return m_iWood; }
        inline int GetStone() { return m_iStone; }
        inline int GetIron() { return m_iIron; }

    private:
        // Individual resource types
        int m_iGold, m_iWood, m_iStone, m_iIron;
};

#endif