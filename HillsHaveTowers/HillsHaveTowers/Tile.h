//*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
// Tile
//
// Purpose:
// Holds information about a single tile and whether or not it is occupied
//
// Credit: Nick
//*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~

#ifndef TILE_H
#define TILE_H

#include "SelectableManager.h"
#include "OgreManualObject.h"
#include "BoundingBox.h"
#include "Terrain.h"

class Tile: public Selectable
{
public:
    //Constructors
	Tile();
	Tile(int p_iX, int p_iY);
    Tile(Ogre::Vector3 p_v3Position);
    Tile(int p_iX, int p_iY, int p_iTileInfo, bool p_bHasResource, bool p_bIsSelectable,
         float p_fTileSize = m_sfDefaultSize);
    Tile(int p_iX, int p_iY, int p_iTileInfo, bool p_bHasResource);
	~Tile();
	
    //Equal operator only compares whether the tiles are in the same spot on the grid
	bool operator==(const Tile& other);
    Ogre::Vector3 GetLocation();
    
    inline bool operator!=(const Tile& other) { return !(*this == other); }
	
    //Getters for the tile's position relative to the grid instead of their actual graphical
    // position
	inline int GetGridX() { return m_iGridX; }
	inline int GetGridY() { return m_iGridY; }
	
    //Getters for the size of the tile
	inline float GetHeight() { return m_fHeight; }

    void SetHeight(float p_fHeight);

    inline Tile* GetPointer() { return this; }
	
    //Setters to let the tile know that if a tower was placed or removed
    // or if a resource was removed (resources can't be placed)
	inline void SetHasTower(bool p_bHasTower) { m_bHasTower = p_bHasTower; }
	inline void SetHasResource(bool p_bHasResource) {  m_bHasResource = p_bHasResource; }
	
	inline bool IsBuildable() { return !(m_bHasTower || m_bHasResource) && m_iTileInfo == 1; }
	inline bool IsOpenPath() { return !((m_iTileInfo == 0) || (m_iTileInfo == 1) || (m_iTileInfo == 5))
	                                  && !m_bHasResource; }

	inline bool IsVillageTile() { return (m_iTileInfo == 4); }
	inline bool IsStartingTile() { return (m_iTileInfo == 3); }

    inline bool IsHorizontalBridge() { return m_iTileInfo == 6; }
    inline bool IsVerticalBridge() { return m_iTileInfo == 7; }

    //IsWalkableTerrain:The player can go anywhere that doesn't have a tower or resource on it
	inline bool IsWalkableTerrain() { return !(m_bHasResource) && m_iTileInfo != 5;}
    inline bool IsWater() { return m_iTileInfo == 5; }

    inline static float GetDefaultSize() { return m_sfDefaultSize; }
    inline static void SetDefaultSize(float p_fSize) { m_sfDefaultSize = p_fSize; }

    inline static float GetXOffset() { return m_sfXOffset; }
    inline static float GetZOffset() { return m_sfZOffset; }

    inline static void SetXOffset(float p_fOffset) { m_sfXOffset = p_fOffset; }
    inline static void SetZOffset(float p_fOffset) { m_sfZOffset = p_fOffset; }
	
	/***********************************************************************************
	*	Added by Pierre
	************************************************************************************/
	virtual const Ogre::AxisAlignedBox& GetBoundingBox() { return m_aabBoundingBox; }; 
	virtual void ShowObject(bool p_bIsShow);

private:

    int m_iGridX, m_iGridY;
	float m_fSize, m_fHeight;
	Ogre::AxisAlignedBox m_aabBoundingBox;

    static float m_sfDefaultSize;
    static float m_sfXOffset, m_sfZOffset;
    /***********************************************************************************/

	bool m_bHasTower, m_bHasResource, m_bIsSelectable;
    
	int m_iTileInfo;
    //TileInfo:
    //0: Unbuildable
    //1: Buildable
    //2: Path
    //3: Path Entrance
    //4: Village
  
};

#endif