#ifndef LEVELPARSER_H
#define LEVELPARSER_H

#include "EnemyWaveSender.h"

#include "tinyxml2.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

class LevelParser
{
public:

    //Reads in a single file and assigns all data to specific data members
    static void ReadFile(std::string p_sName);

    static void ReadLevelFile(std::string p_sName);

    static void SetLevelFile(std::string p_sName);

    //Getters for the data members
    static inline int** GetTerrainLayer() { return m_iTerrainLayer; }
    static inline int** GetResourceLayer() { return m_iResourceLayer; }
    static inline EnemyWaveSender* GetWaveSender() { return m_ewsWaves; }
    static inline std::string GetHeightMapFileName() { return m_sHeightMapName; }
    static inline std::string GetMaterialName() { return m_sMaterialName; }
    static inline int GetNumTilesX() { return m_iXTiles; }
    static inline int GetNumTilesZ() { return m_iZTiles; }
    static inline int GetTerrainXStretch() { return m_iTerrainStretchX; }

	static void Destroy();

private:

    static void ReadWaveFile(std::string p_sName);
    
    static int GetTerrainWidth(tinyxml2::XMLElement* p_xElement);
    static int GetNumElements(tinyxml2::XMLElement* p_xElement, const char* p_cName);

    //Terrain: Stores what kind of terrain is on that tile (unbuildable, buildable, path)
    //Resource: What kind of resource exists on the tile (wood, rock, iron) or if it
    // even has one
    static int** m_iTerrainLayer;
    static int** m_iResourceLayer;

    static int m_iXTiles;
    static int m_iZTiles;

    static int m_iTerrainXOffset;
    static int m_iTerrainZOffset;
    static float m_fTileSize;
    static int m_iTerrainStretchX;

    static std::string m_sHeightMapName;
    static std::string m_sMaterialName;

    static EnemyWaveSender* m_ewsWaves;
};

#endif