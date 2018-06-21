#include "Tile.h"
#include <cstdio>

float Tile::m_sfDefaultSize = 100;
float Tile::m_sfXOffset = 113;
float Tile::m_sfZOffset = 113;

Tile::Tile()
{
    Ogre::Vector3 position(0, 0, 0);

    m_iGridX = -1;
    m_iGridY = -1;

    m_fSize = m_sfDefaultSize;

    m_fHeight = 0;

    m_bHasTower = false;
    m_bHasResource = true;
    m_bIsSelectable = false;
    m_iTileInfo = -1;
}

Tile::Tile(int p_iX, int p_iY)
{
    m_iGridX = p_iX;
    m_iGridY = p_iY;

    m_fSize = m_sfDefaultSize;

    m_fHeight = 0;

    m_fHeight = 0;

    m_bHasTower = false;
    m_bHasResource = false;
    m_bIsSelectable = false;
    m_iTileInfo = 1;
}

Tile::Tile(Ogre::Vector3 p_v3Position)
{
    m_fSize = m_sfDefaultSize;

    m_iGridX = (p_v3Position.x - m_sfXOffset) / m_fSize;
    m_iGridY = (p_v3Position.z - m_sfZOffset) / m_fSize;

    m_fSize = m_sfDefaultSize;

    m_fHeight = 0;

    m_bHasTower = false;
    m_bHasResource = false;
    m_bIsSelectable = false;
    m_iTileInfo = 0;
}

Tile::Tile(int p_iX, int p_iY, int p_iTileInfo, bool p_bHasResource, bool p_bIsSelectable, float p_fTileSize)
{
    m_iGridX = p_iX;
    m_iGridY = p_iY;

    m_fSize = p_fTileSize;
    
    m_fHeight = Terrain::GetHeight(m_fSize * ((float)m_iGridX + 0.5f) + m_sfXOffset,
                                   m_fSize * ((float)m_iGridY + 0.5f) + m_sfZOffset);

    if(m_fHeight == -1)
    {
        m_fHeight = Terrain::GetHeight(m_fSize * ((float)m_iGridX + 0.5f) + m_sfXOffset + 0.1f,
                           m_fSize * ((float)m_iGridY + 0.5f) + m_sfZOffset + 0.1f);
    }

    Ogre::Vector3 position = GetLocation();

    m_bHasTower = false;
    m_bHasResource = p_bHasResource;
    m_iTileInfo = p_iTileInfo;

    m_bIsSelectable = p_bIsSelectable;

    if(m_bIsSelectable)
    {
        float minHeight;
        float maxHeight;

        float tempHeight = Terrain::GetHeight(m_fSize * m_iGridX + m_sfXOffset, m_fSize * m_iGridY + m_sfZOffset);

        maxHeight = std::max(tempHeight, m_fHeight);
        minHeight = std::min(tempHeight, m_fHeight);

        tempHeight = Terrain::GetHeight(m_fSize * (m_iGridX + 1) - 1 + m_sfXOffset, m_fSize * m_iGridY + m_sfZOffset);
        maxHeight = std::max(tempHeight, maxHeight);
        minHeight = std::min(tempHeight, minHeight);

        tempHeight = Terrain::GetHeight(m_fSize * m_iGridX + m_sfXOffset, m_fSize * (m_iGridY + 1) - 1 + m_sfZOffset);
        maxHeight = std::max(tempHeight, maxHeight);
        minHeight = std::min(tempHeight, minHeight);

        tempHeight = Terrain::GetHeight(m_fSize * (m_iGridX + 1) - 1 + m_sfXOffset, m_fSize * (m_iGridY + 1) - 1 + m_sfZOffset);
        maxHeight = std::max(tempHeight, maxHeight);
        minHeight = std::min(tempHeight, minHeight);

        /***********************************************************************************
        *	Added by Pierre
        ************************************************************************************/
        m_aabBoundingBox = Ogre::AxisAlignedBox((position.x - m_fSize/2), minHeight, (position.z - m_fSize/2),
            (position.x + m_fSize/2), maxHeight, (position.z + m_fSize/2));

        m_tSelectableObj = SO_Tile;
        SelectableManager::Add(this);
        /***********************************************************************************/
    }
}

Tile::~Tile()
{
	/***********************************************************************************
	*	Added by Pierre
	************************************************************************************/
    if(m_bIsSelectable)
		SelectableManager::Remove(this);
	/***********************************************************************************/
}

bool Tile::operator==(const Tile& other)
{
    if(m_iGridX == other.m_iGridX && m_iGridY == other.m_iGridY) return true;

    return false;
}

Ogre::Vector3 Tile::GetLocation()
{ 
    float xLoc = m_fSize * ((float)m_iGridX + 0.5f) + m_sfXOffset;
    float zLoc = m_fSize * ((float)m_iGridY + 0.5f) + m_sfZOffset;

    return Ogre::Vector3(xLoc, m_fHeight, zLoc);
}

void Tile::SetHeight(float p_fHeight)
{
     m_fHeight = p_fHeight;

     Ogre::Vector3 position = GetLocation();

     m_aabBoundingBox = Ogre::AxisAlignedBox((position.x - m_fSize/2), m_fHeight-1, (position.z - m_fSize/2),
            (position.x + m_fSize/2), m_fHeight, (position.z + m_fSize/2));
}

/**************************************************************************************
*	Show the bounding box of the entity. (Added by Pierre)
***************************************************************************************/
void Tile::ShowObject(bool p_bIsShow)
{

	// If it not show the bounding box but it want to show the box, show the box
	if(p_bIsShow && !m_bIsShow)
	{
		m_bIsShow = true;
	}

	// If it shows the bounding box but it don't want to show the box, disable it
	else if (!p_bIsShow && m_bIsShow)
	{
		m_bIsShow = false;
	}
}
