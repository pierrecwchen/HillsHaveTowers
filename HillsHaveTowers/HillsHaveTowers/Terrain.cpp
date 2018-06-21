/************************************************************************
Terrain.cpp										Created by Pavel Szczesny

Class for generating terrain from a height map as well as randomizing
the terrain using Perlin noise
************************************************************************/

#include "Terrain.h"
#include "BaseApplication.h"
#include "TileSet.h"
#include <OgreMeshManager.h>
#include <OgreSubMesh.h>
#include <OgrePixelFormat.h>
#include <OgreHardwareVertexBuffer.h>
#include <iostream>
#include <fstream>
#include <time.h>

size_t Terrain::m_iTerrainWidth = 0;
size_t Terrain::m_iTerrainLength = 0;
size_t Terrain::m_iVertexCount = 0;
size_t Terrain::m_iIndexCount = 0;

int Terrain::m_iTerrainScale = 1;

Ogre::Vector3* Terrain::m_pVertices = 0;
Ogre::Vector3* Terrain::m_pNormals = 0;
Ogre::Vector3* Terrain::m_pColours = 0;
Ogre::Vector2* Terrain::m_pTexture = 0;
int* Terrain::m_pIndexes = 0;

Ogre::RaySceneQuery* Terrain::m_pRaySceneQuery = 0;

float** Terrain::m_pPerlinNoise = 0;

Terrain::Terrain()
{
	
}

Terrain::~Terrain()
{
	delete[] m_pVertices;
	delete[] m_pNormals;
	delete[] m_pColours;
	delete[] m_pTexture;
	delete[] m_pIndexes;
	delete[] m_pRaySceneQuery;
}

/************************************************************************
Generate Terrain

Creates a mesh and submesh, sets up vertex and index buffers,
loads mesh into the system so an entity can be created from the mesh
************************************************************************/
void Terrain::GenerateTerrain(std::string p_sFileName, int p_iStretchX)
{
    m_iTerrainScale = p_iStretchX;

	// Create a mesh and sub mesh as this is what the terrain entity will be loaded from
	Ogre::MeshPtr pMesh = Ogre::MeshManager::getSingleton().createManual("Grid", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::SubMesh* pSubMesh = pMesh->createSubMesh();

	CalculateVertices(p_sFileName);
	//GeneratePerlinNoise(7);

	/*int index = 0;
	for (int i = 0; i < m_iTerrainWidth; i++)
	{
		for (int j = 0; j < m_iTerrainLength; j++)
		{
			m_pVertices[index].y = m_pPerlinNoise[i][j] * 255;
			index++;
		}
	}*/
	// Modify the loop iterations to change the number of smoothing passes
	for (int i = 0; i < 2; i++)
	{
		SmoothTerrain(0.75f);
	}

	CalculateNormals();
	CalculateTextureCoordinates();
	//LoadTexture("TestTexture");
	//SetColours();
	SetIndexes();

	// File writing for debugging purposes
	/*std::ofstream myfile("kequals1.txt");
	index = 0;
	for (int i = 0; i < m_iTerrainWidth; i++)
	{
		for (int j = 0; j < m_iTerrainLength; j++)
		{
			index++;
			myfile << pVertices[index] << "\n";
			index++;
			index++;
		}
	}
	myfile.close();*/

	pMesh->sharedVertexData = new Ogre::VertexData();
	pMesh->sharedVertexData->vertexCount = m_iVertexCount;

	Ogre::VertexDeclaration* pVertDecl = pMesh->sharedVertexData->vertexDeclaration;
	// Add vertices to buffer
	size_t offset = 0;
	pVertDecl->addElement(0, offset, Ogre::VertexElementType::VET_FLOAT3, Ogre::VertexElementSemantic::VES_POSITION);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VertexElementType::VET_FLOAT3);
	
	Ogre::HardwareVertexBufferSharedPtr pVertexBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(offset, 
		pMesh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	pVertexBuffer->writeData(0, pVertexBuffer->getSizeInBytes(), m_pVertices, true);
	// Bind to buffer stream
	Ogre::VertexBufferBinding* pBinding = pMesh->sharedVertexData->vertexBufferBinding;
	pBinding->setBinding(0, pVertexBuffer);

	// Add normals to buffer and bind to buffer stream
	offset = 0;
	pVertDecl->addElement(1, offset, Ogre::VertexElementType::VET_FLOAT3, Ogre::VertexElementSemantic::VES_NORMAL);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VertexElementType::VET_FLOAT3);

	pVertexBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(offset, pMesh->sharedVertexData->vertexCount, 
		Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	pVertexBuffer->writeData(0, pVertexBuffer->getSizeInBytes(), m_pNormals, true);
	pBinding->setBinding(1, pVertexBuffer);

	// Add colours to buffer
	/*offset = 0;
	pVertDecl->addElement(2, offset, Ogre::VertexElementType::VET_FLOAT3, Ogre::VertexElementSemantic::VES_DIFFUSE);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VertexElementType::VET_FLOAT3);

	pVertexBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(offset, pMesh->sharedVertexData->vertexCount, 
		Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	pVertexBuffer->writeData(0, pVertexBuffer->getSizeInBytes(), m_pColours, true);
	pBinding->setBinding(2, pVertexBuffer);*/

	// Add texture coordinates to buffer and bind to buffer stream
	offset = 0;
	pVertDecl->addElement(2, offset, Ogre::VertexElementType::VET_FLOAT2, Ogre::VertexElementSemantic::VES_TEXTURE_COORDINATES);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VertexElementType::VET_FLOAT2);

	pVertexBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(offset, pMesh->sharedVertexData->vertexCount, 
		Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	pVertexBuffer->writeData(0, pVertexBuffer->getSizeInBytes(), m_pTexture, true);
	pBinding->setBinding(2, pVertexBuffer);

	// Create index buffer
	Ogre::HardwareIndexBufferSharedPtr pIndexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
		Ogre::HardwareIndexBuffer::IT_32BIT, m_iIndexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	pIndexBuffer->writeData(0, pIndexBuffer->getSizeInBytes(), m_pIndexes, true);

	pSubMesh->useSharedVertices = true;
	pSubMesh->indexData->indexBuffer = pIndexBuffer;
	pSubMesh->indexData->indexCount = m_iIndexCount;
	pSubMesh->indexData->indexStart = 0;

	// Set culling for the terrain
	//pMesh->_setBounds(Ogre::AxisAlignedBox::BOX_INFINITE);
    pMesh->_setBounds(Ogre::AxisAlignedBox(Ogre::Vector3(0,0,0), Ogre::Vector3(m_iTerrainWidth * m_iTerrainScale, 256, m_iTerrainLength * m_iTerrainScale)));
    pMesh->_setBoundingSphereRadius(Ogre::Math::Sqrt(3*10*10));

	// Load mesh so it can be used to create an entity
	pMesh->load();

	/*Ogre::MaterialPtr terrainMaterial = Ogre::MaterialManager::getSingleton().getByName("HillsHaveTowers/AlphaSplatTerrain");
	Ogre::TextureUnitState* baseState = terrainMaterial->getTechnique(0)->getPass(0)->createTextureUnitState();
	baseState->setTextureName("baseTexture", Ogre::TEX_TYPE_2D);*/
}

/************************************************************************
CalaculateVertices

Reads in a height map and plots the vetices for the terrain
************************************************************************/
void Terrain::CalculateVertices(std::string p_sFileName)
{
	Ogre::Image* pImage = new Ogre::Image();
	//pImage->load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	pImage->load(p_sFileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	m_iTerrainWidth = pImage->getWidth();
	m_iTerrainLength = pImage->getHeight();
	m_iVertexCount = m_iTerrainLength * m_iTerrainWidth;

	m_pVertices = new Ogre::Vector3[m_iVertexCount];

	int index = 0;
	for (int i = 0; i < m_iTerrainWidth; i++)
	{
		for (int j = 0; j < m_iTerrainLength; j++)
		{
			m_pVertices[index] = Ogre::Vector3(i * m_iTerrainScale, ((pImage->getColourAt(i, j, 0).b * 255) / 4), j * m_iTerrainScale);
			//m_pVertices[index] = Ogre::Vector3(i, ((pImage->getColourAt(i, j, 0).b * 255) / 3), j);
			index++;
		}
	}
}

/************************************************************************
SmoothTerrain

Uses band smoothing filter to smooth terrain
************************************************************************/
void Terrain::SmoothTerrain(float p_fSmoothFactor)
{
	// Keeps smooth factor between 0 & 1
	p_fSmoothFactor >= 1 ? 0.99 : p_fSmoothFactor;
	p_fSmoothFactor <= 0 ? 0.01 : p_fSmoothFactor;

	//Smoothing pass on vertices: Rows, left to right
	int index = m_iTerrainWidth;
	for (int i = 1; i < m_iTerrainWidth; i++)
	{
		for (int j = 0; j < m_iTerrainLength; j++)
		{
			m_pVertices[index].y = (m_pVertices[index - 1].y * (1 - p_fSmoothFactor)) + (m_pVertices[index].y * p_fSmoothFactor);
			index++;
		}
	}

	//Smoothing pass on vertices: Rows, right to left
	index = m_iVertexCount - m_iTerrainWidth - 1;
	for (int i = m_iTerrainWidth - 1; i > 0; i--)
	{
		for (int j = 0; j < m_iTerrainLength; j++)
		{
			m_pVertices[index].y = (m_pVertices[index + 1].y * (1 - p_fSmoothFactor)) + (m_pVertices[index].y * p_fSmoothFactor);
			index--;
		}
	}

	//Smoothing pass on vertices: Columns, bottom to top
	index = m_iTerrainLength;
	for (int i = 0; i < m_iTerrainWidth; i++)
	{
		for (int j = 1; j < m_iTerrainLength; j++)
		{
			m_pVertices[index].y = (m_pVertices[index - m_iTerrainLength].y * (1 - p_fSmoothFactor)) + (m_pVertices[index].y * p_fSmoothFactor);
			index++;
		}
	}

	//Smoothing pass on vertices: Columns, top to bottom
	index = m_iVertexCount - m_iTerrainLength - 1;
	for (int i = 0; i < m_iTerrainWidth; i++)
	{
		for (int j = m_iTerrainLength - 1; j > 0; j--)
		{
			m_pVertices[index].y = (m_pVertices[index + m_iTerrainLength].y * (1 - p_fSmoothFactor)) + (m_pVertices[index].y * p_fSmoothFactor);
			index--;
		}
	}
}

/************************************************************************
CalaculateNormals

Calculate the normals for the terrain
************************************************************************/
void Terrain::CalculateNormals()
{
	m_pNormals = new Ogre::Vector3[m_iVertexCount];
	int index = 0;
	for (int i = 0; i < m_iTerrainWidth; i++)
	{
		for (int j = 0; j < m_iTerrainLength; j++)
		{
			// Calculation for x = 0 and y = 0 coordinate
			if (i == 0 && j == 0)
			{
				m_pNormals[index] = (m_pVertices[index + 1].crossProduct(m_pVertices[index + m_iTerrainWidth + 1]) +
					m_pVertices[index + m_iTerrainWidth + 1].crossProduct(m_pVertices[index + m_iTerrainWidth])) / 2;
				m_pNormals[index].normalise();
			}
			// Calculation for x = width and y = height coordinate
			else if(i == (m_iTerrainWidth - 1) && j == (m_iTerrainLength - 1))
			{
				m_pNormals[index] = (m_pVertices[index - 1].crossProduct(m_pVertices[index - m_iTerrainWidth - 1]) +
					m_pVertices[index - m_iTerrainWidth - 1].crossProduct(m_pVertices[index - m_iTerrainWidth])) / 2;
				m_pNormals[index].normalise();
			}
			// Calculation for x = 0 and y = height
			else if(i == 0 && j == (m_iTerrainLength - 1))
			{
				m_pNormals[index] = m_pVertices[index + m_iTerrainWidth].crossProduct(m_pVertices[index - 1]);
				m_pNormals[index].normalise();
			}
			// Calculation for  x = width and y = 0
			else if (i == (m_iTerrainWidth - 1) && j == 0)
			{
				m_pNormals[index] = m_pVertices[index - m_iTerrainWidth].crossProduct(m_pVertices[index + 1]);
				m_pNormals[index].normalise();
			}
			// Calculation for first row of coordinates
			else if (i == 0 && j > 0 && j < (m_iTerrainLength - 1))
			{
				m_pNormals[index] = (m_pVertices[index + m_iTerrainWidth].crossProduct(m_pVertices[index - 1]) +
					m_pVertices[index + m_iTerrainWidth + 1].crossProduct(m_pVertices[index + m_iTerrainWidth]) +
					m_pVertices[index + 1].crossProduct(m_pVertices[index + m_iTerrainWidth + 1])) / 3;
				m_pNormals[index].normalise();
			}
			// Calculation for last row of coordinates
			else if (i == (m_iTerrainWidth - 1) && j > 0 && j < (m_iTerrainLength - 1))
			{
				m_pNormals[index] = (m_pVertices[index - m_iTerrainWidth - 1].crossProduct(m_pVertices[index - 1]) +
					m_pVertices[index - m_iTerrainWidth -1].crossProduct(m_pVertices[index - m_iTerrainWidth]) +
					m_pVertices[index - m_iTerrainWidth].crossProduct(m_pVertices[index + 1])) / 3;
				m_pNormals[index].normalise();
			}
			// Calculation for first column of coordinates
			else if (i > 0 && i < (m_iTerrainWidth - 1) && j == 0)
			{
				m_pNormals[index] = (m_pVertices[index - m_iTerrainWidth].crossProduct(m_pVertices[index + 1]) +
					m_pVertices[index + 1].crossProduct(m_pVertices[index + m_iTerrainWidth + 1]) +
					m_pVertices[index + m_iTerrainWidth + 1].crossProduct(m_pVertices[index + m_iTerrainWidth])) / 3;
				m_pNormals[index].normalise();
			}
			// Calculation for last column of coordinates
			else if (i > 0 && i < (m_iTerrainWidth - 1) && j == (m_iTerrainLength - 1))
			{
				m_pNormals[index] = (m_pVertices[index - m_iTerrainWidth].crossProduct(m_pVertices[index - m_iTerrainWidth - 1]) +
					m_pVertices[index- m_iTerrainWidth - 1].crossProduct(m_pVertices[index  - 1]) +
					m_pVertices[index + m_iTerrainWidth].crossProduct(m_pVertices[index - 1])) / 3;
				m_pNormals[index].normalise();
			}
			// Calculation for all inner coordiantes
			else
			{
				m_pNormals[index] = (m_pVertices[index - 1].crossProduct(m_pVertices[index - m_iTerrainWidth - 1]) +
					m_pVertices[index - m_iTerrainWidth - 1].crossProduct(m_pVertices[index - m_iTerrainWidth]) +
					m_pVertices[index - m_iTerrainWidth].crossProduct(m_pVertices[index + 1]) +
					m_pVertices[index + 1].crossProduct(m_pVertices[index + m_iTerrainWidth + 1]) +
					m_pVertices[index + m_iTerrainWidth + 1].crossProduct(m_pVertices[index + m_iTerrainWidth]) +
					m_pVertices[index + m_iTerrainWidth].crossProduct(m_pVertices[index - 1])) / 6;
				m_pNormals[index].normalise();
			}
			index++;
		}
	}
}

/************************************************************************
CalculateTextureCoordinates

Texture coordinates calculated so that textures cover the whoe terrain
with the scale ration of 1:1
************************************************************************/
void Terrain::CalculateTextureCoordinates()
{	
	m_pTexture = new Ogre::Vector2[m_iVertexCount];
	int index = 0;
	for (int i = 0; i < m_iTerrainWidth; i++)
	{
		for (int j = 0; j < m_iTerrainLength; j++)
		{
			m_pTexture[index] = Ogre::Vector2( (float)j / (m_iTerrainLength - 1) , (float)i / (m_iTerrainWidth - 1));
			index++;
		}
	}
}

/************************************************************************
LoadTexture

Code for manually loading textures into ogre
************************************************************************/
void Terrain::LoadTexture(Ogre::String p_strName)
{

	Ogre::TexturePtr pTexture = Ogre::TextureManager::getSingleton().createManual(
		p_strName,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TextureType::TEX_TYPE_2D,
		m_iTerrainWidth,
		m_iTerrainLength,
		1,
		0,
		Ogre::PixelFormat::PF_A8B8G8R8,
		Ogre::TU_DYNAMIC_WRITE_ONLY);
}

/************************************************************************
SetColours

Add uniform colour to terrain
************************************************************************/
void Terrain::SetColours()
{
	Ogre::RenderSystem* renderSystem = Ogre::Root::getSingleton().getRenderSystem();
	m_pColours = new Ogre::Vector3[m_iVertexCount];
	int index = 0;

	for (int i = 0; i < m_iVertexCount; i++)
	{
		m_pColours[index] = Ogre::Vector3(0.5f, 0.5f, 0.5f);
		index++;
	}
}

/************************************************************************
SetIndexes

Set indexes for use with an index buffer
************************************************************************/
void Terrain::SetIndexes()
{
	m_iIndexCount = (m_iTerrainWidth - 1) * (m_iTerrainLength - 1) * 6;
	m_pIndexes = new int[m_iIndexCount];

	int index = 0;
	for (int i = 0; i < m_iTerrainWidth - 1; i++)
	{
		for (int j = 0; j < (m_iTerrainLength - 1); j++)
		{
			m_pIndexes[index] = (i * m_iTerrainWidth) + j;
			index++;
			m_pIndexes[index] = (i * m_iTerrainWidth) + (j + 1);
			index++;
			m_pIndexes[index] = ((i + 1) * m_iTerrainWidth) + j;
			index++;
			m_pIndexes[index] = ((i + 1) * m_iTerrainWidth) + j;
			index++;
			m_pIndexes[index] = (i * m_iTerrainWidth) + (j + 1);
			index++;
			m_pIndexes[index] = ((i + 1) * m_iTerrainWidth) + (j + 1);
			index++;
		}
	}
}

/************************************************************************
Get Height

Returns the height of a specific point on the terrain
************************************************************************/
float Terrain::GetHeight(float p_fX, float p_fZ)
{
	// Create ray scene query object
	m_pRaySceneQuery = Ogre::Root::getSingletonPtr()->getSceneManager(
		BaseApplication::GetSceneManagerName())->createRayQuery(Ogre::Ray());
	if (m_pRaySceneQuery == 0)
	{
		return -1.0f;
	}
	m_pRaySceneQuery->setSortByDistance(true);
    m_pRaySceneQuery->setQueryMask(1<<5);

	// Create ray used for testing
	// If ray fails to be created or doesn't hit anything,
	// then return -1
	Ogre::Ray ray(Ogre::Vector3(p_fX, 300.0f, p_fZ), Ogre::Vector3(0.0f, -1.0f, 0.0f));
	if (m_pRaySceneQuery != 0)
	{
		m_pRaySceneQuery->setRay(ray);
		if (m_pRaySceneQuery->execute().size() <= 0)
		{
            return -1.0f;
		}
	}
	else
	{
		return -1.0f;
	}

	float fClosestDistance = -1.0f;
	Ogre::Vector3 vClosestResult;
	Ogre::RaySceneQueryResult &queryResult = m_pRaySceneQuery->getLastResults();

    float highestPoint = 0;

	for (int i = 0; i < queryResult.size(); i++)
	{
		// Stop checking if we have found a raycast hit that is closer
        // than all remaining entities. This check isn't really necessary for this scenario
		// as we only care about the one entity (the terrain).
		if (fClosestDistance >= 0.0f && fClosestDistance < queryResult[i].distance)
		{
			break;
		}

		if (queryResult[i].movable != 0 && queryResult[i].movable->getMovableType().compare("Entity") == 0)
		{
			Ogre::Entity* pEntity = static_cast<Ogre::Entity*>(queryResult[i].movable);
			// Check to make sure we have hit the terrain
			// If other enities are hit move to the next itteration
            std::string entityName = pEntity->getName();

			if (entityName.substr(0, 6) == "Bridge")
			{
				highestPoint = std::max(highestPoint, TileSet::GetPathHeight());
			}
			else if (entityName == "Terrain")
			{
                //NICK (modified Pavel's code to check only two triangles as opposed to all triangles)~*~*~*~*~*~*~*~*~*~*~*~*~*~*
                bool bNewClosest = false;           

                //Since we're shooting a ray directly down, we can actually determine which 4 vertices the ray
                // will go between since the vertices are spaced out by 1 multiplied by the terrain stretch units.
                int index = ((int)p_fX / m_iTerrainScale) * m_iTerrainWidth + ((int)p_fZ / m_iTerrainScale);

                //We check the two triangles that go between the four vertices
                std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, m_pVertices[index], m_pVertices[index + 1], 
                    m_pVertices[index + m_iTerrainWidth], true, false);

                //If the first one hit, we set it as distance to return 
                if (hit.first)
                {
                    highestPoint = std::max(highestPoint, ray.getPoint(hit.second).y);
                }
				else
				{
                    //If the first check didn't hit, we check the other triangle
					hit = Ogre::Math::intersects(ray, m_pVertices[index + m_iTerrainWidth], m_pVertices[index + 1], 
						m_pVertices[index + m_iTerrainWidth + 1], true, false);

					if (hit.first)
					{
						highestPoint = std::max(highestPoint, ray.getPoint(hit.second).y);
					}
				}

                //~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
			}
		}
	}
	if (highestPoint >= 0.0f)
	{
        //We return the height at the point of intersection
		return highestPoint;
	}
	else
	{
		return -1.0f;
	}
}

//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
// NICK: Find if there are any intersections with the triangles within a tile
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
std::pair<bool, Ogre::Real> Terrain::IntersectsWithTile(Ogre::Ray p_rRay, int p_iX, int p_iZ, float p_fTileSize, float p_fXOffset, float p_fZOffset)
{
    //For every triangle that files within the tile
    for(int x = (p_iX * p_fTileSize) / m_iTerrainScale; x < (p_iX * p_fTileSize + p_fTileSize) / m_iTerrainScale; x++)
    {
        for(int z = (p_iZ * p_fTileSize) / m_iTerrainScale; z < (p_iZ * p_fTileSize + p_fTileSize) / m_iTerrainScale; z++)
        {
            //We check the vertices at that index
            int vIndex = (p_fXOffset + x) * m_iTerrainWidth + z + p_fZOffset;

            //If the ray intersects with either of the triangles
            std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(p_rRay, m_pVertices[vIndex], m_pVertices[vIndex + 1], 
                m_pVertices[vIndex + m_iTerrainWidth], true, false);

            //We return a hit
            if (hit.first)
            {
                return hit;
            }

            hit = Ogre::Math::intersects(p_rRay, m_pVertices[vIndex + m_iTerrainWidth], m_pVertices[vIndex + 1], 
                m_pVertices[vIndex + m_iTerrainWidth + 1], true, false);

            if (hit.first)
            {
                return hit;
            }
        }
    }
    std::pair<bool, Ogre::Real> temp(false, 0);

    //If no triangle was intersected, then we return an empty pair
    return temp;
}

/************************************************************************
GenerateWhiteNoise
************************************************************************/
float** Terrain::GenerateWhiteNoise()
{
	srand(0);
	float** pNoise = new float*[m_iTerrainWidth];
	for (int i = 0; i < m_iTerrainWidth; ++i)
	{
		pNoise[i] = new float[m_iTerrainLength];
	}

	for (int i = 0; i < m_iTerrainWidth; ++i)
	{
		for (int j = 0; j < m_iTerrainLength; ++j)
		{
			pNoise[i][j] = (float)((rand() % 100)) / 100.0f;
		}
	}

	return pNoise;
}

/************************************************************************
GenerateSmoothNoise
************************************************************************/

float** Terrain::GenerateSmoothNoise(float** p_pBaseNoise, int p_iOctave)
{
	float** pNoise = new float*[m_iTerrainWidth];
	for (int i = 0; i < m_iTerrainWidth; ++i)
	{
		pNoise[i] = new float[m_iTerrainLength];
	}

	int iSamplePeriod = 1 << p_iOctave; // calculates 2 ^ k
	float fSampleFrequency = 1.0f / iSamplePeriod;

	for (int i = 0; i < m_iTerrainWidth; ++i)
	{
		//calculate the horizontal sampling indices
		int iSample_i0 = (i / iSamplePeriod) * iSamplePeriod;
		int iSample_i1 = (iSample_i0 + iSamplePeriod) % m_iTerrainWidth; //wrap around
		float fHorizontalBlend = (i - iSample_i0) * fSampleFrequency;

		for (int j = 0; j < m_iTerrainLength; ++j)
		{
			//calculate the vertical sampling indices
			int iSample_j0 = (j / iSamplePeriod) * iSamplePeriod;
			int iSample_j1 = (iSample_j0 + iSamplePeriod) % m_iTerrainLength; //wrap around
			float fVerticalBlend = (j - iSample_j0) * fSampleFrequency;
 
			//blend the top two corners
			float fTop = Interpolate(p_pBaseNoise[iSample_i0][iSample_j0], p_pBaseNoise[iSample_i1][iSample_j0], fHorizontalBlend);
 
			//blend the bottom two corners
			float fBottom = Interpolate(p_pBaseNoise[iSample_i0][iSample_j1], p_pBaseNoise[iSample_i1][iSample_j1], fHorizontalBlend);
 
			//final blend
			pNoise[i][j] = Interpolate(fTop, fBottom, fVerticalBlend);
		}
	}

	return pNoise;
}

/************************************************************************
GeneratePerlinNoise
************************************************************************/
void Terrain::GeneratePerlinNoise(int p_pOctaveCount)
{
	// Generate white noise
	float** pBaseNoise = GenerateWhiteNoise();

	float*** pSmoothNoise = new float**[p_pOctaveCount];
	for (int i = 0; i < m_iTerrainWidth; ++i)
	{
		pSmoothNoise[i] = new float*[m_iTerrainWidth];
		for (int j = 0; j < m_iTerrainLength; ++j)
		{
			pSmoothNoise[i][j] = new float[m_iTerrainLength];
		}
	}

	float fPersistence = 0.5f;

	// Generte smooth noise
	for (int i = 0; i < p_pOctaveCount; ++i)
	{
		pSmoothNoise[i] = GenerateSmoothNoise(pBaseNoise, i);
	}

	float fAmplitude = 1.0f;
	float fTotalAmplitude = 0.0f;

	m_pPerlinNoise = new float*[m_iTerrainWidth];
	for (int i = 0; i < m_iTerrainWidth; ++i)
	{
		m_pPerlinNoise[i] = new float[m_iTerrainLength];
	}

	// Blend noise together
	for (int octave = p_pOctaveCount - 1; octave >= 0; --octave)
	{
		for (int i = 0; i < m_iTerrainWidth; ++i)
		{
			fAmplitude *= fPersistence;
			fTotalAmplitude += fAmplitude;

			for (int j = 0; j < m_iTerrainLength; ++j)
			{
				m_pPerlinNoise[i][j] = 0;
				m_pPerlinNoise[i][j] += pSmoothNoise[octave][i][j] * fAmplitude;
			}
		}
	}

	// normalization
	for (int i = 0; i < m_iTerrainWidth; ++i)
	{
		for (int j = 0; j < m_iTerrainLength; ++j)
		{
			m_pPerlinNoise[i][j] /= fTotalAmplitude;
		}
	}
}

/************************************************************************
Interpolate
************************************************************************/
float Terrain::Interpolate(float p_fX0, float p_fX1, float p_fAlpha)
{
	return p_fX0 * (1- p_fAlpha) + p_fAlpha * p_fX1;
}

/************************************************************************
Update
************************************************************************/
void Terrain::Update(const Ogre::FrameEvent& p_evtDelta)
{
    
}

/************************************************************************
Destroy

Delete the terrain and clean up
************************************************************************/
void Terrain::Destroy()
{
	Ogre::MeshManager::getSingleton().destroyAllResourcePools();
	Ogre::TextureManager::getSingleton().destroyAllResourcePools();

	delete[] m_pVertices;
	delete[] m_pNormals;
	delete[] m_pColours;
	delete[] m_pTexture;
	delete[] m_pIndexes;

	delete m_pRaySceneQuery;
}