//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
//Village.h
//
//PURPOSE:
//Creates a village with animated villagers walking around. Amount of villagers is
// related to how many villagers are left in the village
//
//All code here was written by Nick unless stated otherwise
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*

#ifndef VILLAGE_H
#define VILLAGE_H

#include "Model.h"
#include "Tile.h"

class Village
{
public:

    static Village* Instance();
    static void Destroy() { delete m_vVillage; m_vVillage = NULL; }

    //Modifiers which change the amount of villagers in the village
    inline void Add() { m_iVillagerCount++; }
    int Remove(int p_iCount);

    //Gets the current population
    inline int GetPopulation() { return m_iVillagerCount; }

    //Update method which moves villagers around
    void Update(const Ogre::FrameEvent& p_feEvent);


private:

    Village();
	~Village();

    static Village* m_vVillage;

    //The tile the village is centered around and the houses that are built on it
    Tile* m_tVillageTile;
    Model** m_mHouses;

    int m_iVillagerCount;

    //Counter which keeps track of how long since it's fired off an animated villager
    float m_fTimeSinceLastFire;

    //List of villager models
    std::list<Model*> m_lMovingVillagers;
};

#endif