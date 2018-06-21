//---------------------------------------------------------------------------
// Terrain.h										Created by Pavel Szczesny
//
// Class for generating terrain from a height map
//---------------------------------------------------------------------------
#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>

//#include "Patch.h"
//
//// Various Pre-Defined map sizes & their #define counterparts:
//
//#define MAP_512
//
//#ifdef MAP_2048
//
//// ------- 2048x2048 MAP -------
//#define MAP_SIZE (2048)
//#define NUM_PATCHES_PER_SIDE (32)
//
//#else
//#ifdef MAP_1024
//
//// ------- 1024x1024 MAP -------
//#define MAP_SIZE (1024)
//#define NUM_PATCHES_PER_SIDE (16)
//
//#else
//
//// ------- 512x512 MAP -------
//#define MAP_SIZE (512)
//#define NUM_PATCHES_PER_SIDE (8)
//
//#endif
//#endif

class Terrain
{
public:
	/************************************************************************
	Public Functions
	************************************************************************/
	Terrain();
	~Terrain();
	static void GenerateTerrain(std::string p_sFileName, int p_iStretchX);
	static float GetHeight(float p_fX, float p_fZ);
	void Update(const Ogre::FrameEvent& p_evtDelta);

    //~*~*~*~*~*~*~*~*~*~
    //ADDED BY NICK
    //~*~*~*~*~*~*~*~*~*~
    static std::pair<bool, Ogre::Real> IntersectsWithTile(Ogre::Ray p_rRay, int p_iX, int p_iZ, float p_fTileSize, float p_fXOffset, float p_fZOffset);
    //~*~*~*~*~*~*~*~*~*~

    static inline int GetLength() { return m_iTerrainLength * m_iTerrainScale; }
    static inline int GetWidth() { return m_iTerrainWidth * m_iTerrainScale; }

	/*static int GetNextTriNode() { return m_iNextTriNode; }
	static void SetNextTriNode(int pNextNode) { m_iNextTriNode = pNextNode; }
	static TriTreeNode* AllocateTri();
	static void Reset();
	static void Tesselate();*/

	static void Destroy();

private:
	/************************************************************************
	Private Functions
	************************************************************************/
	static void CalculateVertices(std::string p_sFileName);
	static void SmoothTerrain(float p_fSmoothFactor); // Use values between 0 & 1
	static void CalculateNormals();
	static void CalculateTextureCoordinates();
	static void LoadTexture(Ogre::String p_strName);
	static void SetColours();
	static void SetIndexes();

	static float** GenerateWhiteNoise();
	static float** GenerateSmoothNoise(float** p_pBaseNoise, int p_iOctave);
	static void GeneratePerlinNoise(int p_iOctaveCount);
	static float Interpolate(float p_fX0, float p_fX1, float p_fAlpha);

	/************************************************************************
	Private Members
	************************************************************************/
	static size_t m_iTerrainLength;
	static size_t m_iTerrainWidth;
	static size_t m_iVertexCount;
	static size_t m_iIndexCount;

    static int m_iTerrainScale;

	static Ogre::Vector3* m_pVertices;
	static Ogre::Vector3* m_pNormals;
	static Ogre::Vector3* m_pColours;
	static Ogre::Vector2* m_pTexture;
	static int* m_pIndexes;

	static Ogre::RaySceneQuery* m_pRaySceneQuery;

	//static std::vector<Ogre::Vector2*> m_vNoise;
	static float** m_pPerlinNoise;

	/*static Patch m_aPatches[NUM_PATCHES_PER_SIDE][NUM_PATCHES_PER_SIDE];
	static int m_iNextTriNode;
	static TriTreeNode m_tTriPool[25000];*/
};
#endif