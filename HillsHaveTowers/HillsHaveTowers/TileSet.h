//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
//TileSet.h
//
//PURPOSE:
//Stores all of the tiles and processes them for paths and other uses
//
//All code here was written by Nick unless stated otherwise
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*

#ifndef TILESET_H
#define TILESET_H

#include <iostream>
#include <list>

#include "Tile.h"

using namespace std;

//TILE INFO:
//0: not path
//1: path
//2: blocked path
//3: starting
//4: village

class TileSet
{
public:

    //Sets the tiles along with the grid height and length
    static void SetTiles(Tile*** p_tTiles, int p_iGridLength, int p_iGridHeight);
    static void GenerateTiles(int** p_iTerrainLayer, int** p_iResourceLayer, int p_iX, int p_iY);

    //Calculates the shortest path with respect to the enemy pathways
    static void FindShortestPath();

    //Finds the shortest path for the player, with respect to resources and changes in height
    static list<Ogre::Vector3> GetShortestPathFromTo(Tile p_tStartTile,  Tile p_tEndTile, int p_iTileDivisions);

    static inline list<Tile> GetShortestPath() { return m_lShortestPath; }
    
    //Tile Getters
    static Tile* GetTileAt(int p_iX, int p_iY) { return m_tTiles[p_iX][p_iY]; }
    static Tile* GetTileAt(Ogre::Vector3 p_v3Location);
    static inline Tile* GetVillageTile() { return &m_tVillageTile; }
    static inline Tile* GetStartingTile() { return &m_tStartTile; }

    static bool HasWater() { return m_bHasWater; }

    // Get the height of the path (used in calculation for adjusting tower range based on height of terrain)
    static inline float GetPathHeight() { return m_tVillageTile.GetHeight(); }
    
    static int GetWidth() { return m_iGridLength; }
    static int GetLength() { return m_iGridHeight; }

    static vector<Ogre::AxisAlignedBox> GetWaterBoundingBoxes() { return m_vWaterAABBList; }

	static void Destroy();

private:

    static void GenerateWaterBoundingBoxes();
    static void GenerateBridges();

    //Stores the current shortest enemy path
    static list<Tile> m_lShortestPath;

    //Helper methods for FindShortestPath and GetShortestPathFromTo respectively
    static bool CheckEnemyTile(Tile p_tThisTile, Tile p_tStartTile, int** p_pTilesTraveled, Tile*** p_pTiles, int p_iSteps);
    static bool CheckPlayerTile(Tile p_tThisTile, Tile p_tStartTile, float** p_pTilesTraveled, Tile*** p_pTiles, float p_fStepsToTile);

    //Helper method which just shortens up names a bit
    static float GetWeightBetweenTiles(Tile p_tFirst, Tile p_tSecond);

    //the tile list
    static Tile*** m_tTiles;

    //helper methods for the pathfinding methods
    static list<Tile> m_lBufferList;
    static list<Tile> m_lCurrentList;

    static vector<Ogre::AxisAlignedBox> m_vWaterAABBList;

    static Tile m_tVillageTile;
    static Tile m_tStartTile;

    static int m_iGridLength, m_iGridHeight;

    static bool m_bHasWater;
};

#endif