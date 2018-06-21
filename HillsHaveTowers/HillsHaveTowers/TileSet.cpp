//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
//TileSet.cpp
//
//PURPOSE:
//Stores all of the tiles and processes them for paths and other uses
//
//All code here was written by Nick unless stated otherwise
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*

#include "TileSet.h"
#include "BoundingBox.h"
#include "SceneryManager.h"

list<Tile> TileSet::m_lShortestPath;
Tile*** TileSet::m_tTiles;
list<Tile> TileSet::m_lBufferList;
list<Tile> TileSet::m_lCurrentList;
Tile TileSet::m_tVillageTile;
Tile TileSet::m_tStartTile;
int TileSet::m_iGridHeight;
int TileSet::m_iGridLength;
vector<Ogre::AxisAlignedBox> TileSet::m_vWaterAABBList;
bool TileSet::m_bHasWater = false;

//2d array constructor
void TileSet::SetTiles(Tile*** p_tTiles, int p_iGridLength, int p_iGridHeight)
{
    m_iGridLength = p_iGridLength;
    m_iGridHeight = p_iGridHeight;

    m_tTiles = new Tile**[m_iGridLength];

    for(int i = 0; i < m_iGridLength; i++)
    {
        m_tTiles[i] = new Tile*[m_iGridHeight];
    }

    for (int i = 0; i < m_iGridLength; ++i)
        for (int j = 0; j < m_iGridHeight; ++j)
            m_tTiles[i][j] = NULL;

    //Every value taken in from the 2d array is assigned to the m_tTiles 2d array. The m_iTilesTraveled array
    // gets all zeros to signify that no path has been created yet.
    for(int k = 0; k < m_iGridLength; k++)
    {
        for(int j = 0; j < m_iGridHeight; j++)
        {
            m_tTiles[j][k] = p_tTiles[j][k];
        }
    }

    bool villageFound = false;
    bool startFound = false;

    //The m_tTiles array is iterated through until the villageTile and the startTile have been identified
    while(!villageFound && !startFound)
    {
        for(int k = 0; k < m_iGridLength && (!villageFound || !startFound); k++)
        {
            for(int j = 0; j < m_iGridHeight && (!villageFound || !startFound); j++)
            {
                if(m_tTiles[j][k]->IsVillageTile())
                {
                    m_tVillageTile = *m_tTiles[j][k];
                    villageFound = true;
                }
                else if (m_tTiles[j][k]->IsStartingTile())
                {
                    m_tStartTile = *m_tTiles[j][k];
                    startFound = true;
                }
            }
        }
    }
}

void TileSet::GenerateTiles(int** p_iTerrainLayer, int** p_iResourceLayer, int p_iX, int p_iY)
{
    m_iGridLength = p_iX;
    m_iGridHeight = p_iY;

    m_tTiles = new Tile**[m_iGridLength];

    for(int i = 0; i < m_iGridLength; i++)
    {
        m_tTiles[i] = new Tile*[m_iGridHeight];
    }

    bool hasBridges = false;

    //Every value taken in from the 2d array is assigned to the m_tTiles 2d array. The m_iTilesTraveled array
    // gets all zeros to signify that no path has been created yet.
    for(int k = 0; k < m_iGridLength; k++)
    {
        for(int j = 0; j < m_iGridHeight; j++)
        {
            
            m_tTiles[j][k] = new Tile(j, k, p_iTerrainLayer[j][k], (p_iResourceLayer[j][k] != 0), true);
            
            if(p_iTerrainLayer[j][k] == 5)
            {
                m_bHasWater = true;
            }
            else if(p_iTerrainLayer[j][k] == 6 || p_iTerrainLayer[j][k] == 7)
            {
                hasBridges = true;
            }
        }
    }

    bool villageFound = false;
    bool startFound = false;

    //The m_tTiles array is iterated through until the villageTile and the startTile have been identified
    while(!villageFound && !startFound)
    {
        for(int k = 0; k < m_iGridLength && (!villageFound || !startFound); k++)
        {
            for(int j = 0; j < m_iGridHeight && (!villageFound || !startFound); j++)
            {
                if(m_tTiles[j][k]->IsVillageTile())
                {
                    m_tVillageTile = *m_tTiles[j][k];
                    villageFound = true;
                }
                else if (m_tTiles[j][k]->IsStartingTile())
                {
                    m_tStartTile = *m_tTiles[j][k];
                    startFound = true;
                }
            }
        }
    }

    if(m_bHasWater)
    {
        GenerateWaterBoundingBoxes();
    }
    if(hasBridges)
    {
        GenerateBridges();
    }
}

//Converts the position to a temporary tile and then asks the coordinate tile getter for the proper tile
Tile* TileSet::GetTileAt(Ogre::Vector3 p_v3Location)
{
    Tile tempTile(p_v3Location);

    return GetTileAt(tempTile.GetGridX(), tempTile.GetGridY());
}

void TileSet::GenerateWaterBoundingBoxes()
{
    for(int k = 0; k < m_iGridHeight; k++)
    {
        for(int j = 0; j < m_iGridLength; j++)
        {
            if(m_tTiles[j][k]->IsWater())
            {
                m_vWaterAABBList.push_back(Ogre::AxisAlignedBox((m_tTiles[j][k]->GetLocation().x - Tile::GetDefaultSize()/2),
                                                                 -1000,
                                                                (m_tTiles[j][k]->GetLocation().z - Tile::GetDefaultSize()/2),
                                                                (m_tTiles[j][k]->GetLocation().x + Tile::GetDefaultSize()/2), 
                                                                1000, 
                                                                (m_tTiles[j][k]->GetLocation().z + Tile::GetDefaultSize()/2)));
            }
        }
    }
}

void TileSet::GenerateBridges()
{
    for(int k = 0; k < m_iGridHeight; k++)
    {
        for(int j = 0; j < m_iGridLength; j++)
        {
            if(m_tTiles[j][k]->IsHorizontalBridge())
            {
                m_tTiles[j][k]->SetHeight(GetPathHeight());
                SceneryManager::Instance()->AddBridge(Tile(j, k), false);
            }
            else if(m_tTiles[j][k]->IsVerticalBridge())
            {
                m_tTiles[j][k]->SetHeight(GetPathHeight());
                SceneryManager::Instance()->AddBridge(Tile(j, k), true);
            }
        }
    }
}

//The checkEnemyTile method checks if the current tile is the start tile (seeing as the search is started
// from the village tile). If it is, we've found the start tile so we return true. If not, we check
// if the tile is even a valid tile, before checking if that tile was already visited, or if it's even
// a path tile. If it's valid and wasn't visited, we add it to the buffer list to check it later, and 
// we assign that tile a number to signify that it's being visited. The number represents how many
// steps it took to get to that tile from the village.
bool TileSet::CheckEnemyTile(Tile p_tThisTile, Tile p_tStartTile, int** p_pTilesTraveled, Tile*** p_pTiles, int p_iSteps)
{
    bool tileWasVisited = p_pTilesTraveled[p_tThisTile.GetGridX()][p_tThisTile.GetGridY()] != 0;

    if(p_tThisTile == p_tStartTile)
    {
        return true;
    }
    else if (p_pTiles[p_tThisTile.GetGridX()][p_tThisTile.GetGridY()]->IsOpenPath() && !tileWasVisited)
    {
        m_lBufferList.push_back(p_tThisTile);
        p_pTilesTraveled[p_tThisTile.GetGridX()][p_tThisTile.GetGridY()] = p_iSteps + 1;
    }

    return false;
}

void TileSet::FindShortestPath()
{ 
    //The first tile we visit is our Village Tile, which is where we expand out from
    m_lCurrentList.push_back(m_tVillageTile);

    //We set up our tilesTraveled array which keeps track of which tiles we've been to and which ones
    // we haven't been to.
    int** tilesTraveled = new int*[m_iGridLength];

    for(int p = 0; p < m_iGridLength; p++)
    {
        tilesTraveled[p] = new int[m_iGridHeight];
    }

    for(int k = 0; k < m_iGridLength; k++)
    {
        for(int j = 0; j < m_iGridHeight; j ++)
        {
            tilesTraveled[j][k] = 0;
        }
    }

    tilesTraveled[m_tVillageTile.GetGridX()][m_tVillageTile.GetGridY()] = 1;


    //We haven't found the start and we only took our first step at the village
    bool startFound = false;
    int steps = 1;

    while(!startFound)
    {
        //We create an iterator for the current list
        list<Tile>::iterator it;

        //And booleans for looking at each adjacent tile
        bool startIsUp;
        bool startIsLeft;
        bool startIsRight;
        bool startIsDown;

        //For every tile on the current list
        for(it = m_lCurrentList.begin(); it != m_lCurrentList.end() && !startFound; it++)
        {
            Tile currentTile = *it;

            //We check if every adjacent tile to see if it's the start. Since the method doesn't know which
            // direction we came from, we just give it the amount of steps so that it knows how many steps
            // it took to get there.
            if(currentTile.GetGridY() < (m_iGridHeight - 1))
            {
                startIsUp = CheckEnemyTile(*m_tTiles[currentTile.GetGridX()][currentTile.GetGridY() + 1],
                                      m_tStartTile, tilesTraveled, m_tTiles, steps);
            }

            if(currentTile.GetGridX() > 0)
            {
                startIsLeft = CheckEnemyTile(*m_tTiles[currentTile.GetGridX() - 1][currentTile.GetGridY()],
                                        m_tStartTile, tilesTraveled, m_tTiles, steps);
            }

            if(currentTile.GetGridX() < (m_iGridLength - 1))
            {
                startIsRight = CheckEnemyTile(*m_tTiles[currentTile.GetGridX() + 1][currentTile.GetGridY()],
                                         m_tStartTile, tilesTraveled, m_tTiles, steps);
            }

            if(currentTile.GetGridY() > 0)
            {
                startIsDown = CheckEnemyTile(*m_tTiles[currentTile.GetGridX()][currentTile.GetGridY() - 1],
                                        m_tStartTile, tilesTraveled, m_tTiles, steps);
            }

            //If any of those m_tTiles we checked is the starting tile, then we've created our path and we exit
            // the loop
            if(startIsUp || startIsLeft || startIsRight || startIsDown)
            {
                startFound = true;
            }
        }

        //Whether or not we found the start of our path, we 
        steps++;
        m_lCurrentList.clear();
        m_lCurrentList = list<Tile>(m_lBufferList);
        m_lBufferList.clear();

        //printPathSheet();
    }

    //We clear our current list for the next time we pass through our path finding loop.
    m_lCurrentList.clear();

    //We create our queue, which is the path the enemies will follow. We hand it the starting tile.
    list<Tile> shortestPath;
    shortestPath.push_back(m_tStartTile);

    //We create ints that hold the coordinates of our starting tile. This will determine where we
    // are standing as we hand m_tTiles to the queue.
    int currentX = m_tStartTile.GetGridX();
    int currentY = m_tStartTile.GetGridY();

    //While the village tile has not been reached.
    while(steps != 1)
    {
        //If the coordinates are valid
        if(currentX > 0)
        {
            //If the adjacent tile has a step count of one less then our current tile
            if(tilesTraveled[currentX - 1][currentY] == (steps - 1))
            {
                //We move to that tile.
                currentX--;
            }
        }
        if(currentX < (m_iGridLength - 1))
        {
            if(tilesTraveled[currentX + 1][currentY] == (steps - 1))
            {
                currentX++;
            }
        }
        if(currentY > 0)
        {
            if(tilesTraveled[currentX][currentY - 1] == (steps - 1))
            {
                currentY--;
            }
        }
        if(currentY < (m_iGridHeight - 1))
        {
            if(tilesTraveled[currentX][currentY + 1] == (steps - 1))
            {
                currentY++;
            }
        }

        //Whatever tile we moved to gets added to the list and we
        // count down our steps
        shortestPath.push_back(*m_tTiles[currentX][currentY]);
        steps--;
    }

    //We clean up after our pathway for the next time we pathfind

    //The path is returned
    m_lShortestPath = shortestPath;
}


//The CheckPlayerTile method is similar to the CheckEnemyTile method with slight modifications
bool TileSet::CheckPlayerTile(Tile p_tThisTile, Tile p_tStartTile, float** p_pTilesTraveled, Tile*** p_pTiles, float p_fStepsToTile)
{
    //These variables are here to just shorten up the naming scheme
    float currentSteps = p_pTilesTraveled[p_tThisTile.GetGridX()][p_tThisTile.GetGridY()];
    bool tileWasVisited = p_pTilesTraveled[p_tThisTile.GetGridX()][p_tThisTile.GetGridY()] != 0;

    //If this tile is the start tile
    if(p_tThisTile == p_tStartTile)
    {
        //We identify the distance we needed to get to this tile and then return true
        p_pTilesTraveled[p_tThisTile.GetGridX()][p_tThisTile.GetGridY()] = p_fStepsToTile;
        return true;
    }
    //Else, if the player can walk on the tile, and if there isn't a shorter way of getting to it
    // nor has it been visited at all
    else if (p_pTiles[p_tThisTile.GetGridX()][p_tThisTile.GetGridY()]->IsWalkableTerrain() &&
            (p_fStepsToTile < currentSteps || !tileWasVisited))
    {
        //We add this tile to the list of tiles we need to check and we find how far it took us to walk
        // to it
        m_lBufferList.push_back(p_tThisTile);
        p_pTilesTraveled[p_tThisTile.GetGridX()][p_tThisTile.GetGridY()] = p_fStepsToTile;
    }

    return false;
}

float TileSet::GetWeightBetweenTiles(Tile p_tFirst, Tile p_tSecond)
{ 
    return p_tFirst.GetLocation().distance(p_tSecond.GetLocation());
}

//The player pathfinding method is similar to the enemy's except that A) The tiles can be subdivided allowing for
// more accurate movement and B) The player can walk off the path.
list<Ogre::Vector3> TileSet::GetShortestPathFromTo(Tile p_tStartTile,  Tile p_tEndTile, int p_iTileDivisions)
{
    //Because we need to use a subdivided tileset, we need to create new arrays to hold our tiles and
    // our tile traversal chart
    int dividedGridLength = m_iGridLength * p_iTileDivisions;
    int dividedGridHeight = m_iGridHeight * p_iTileDivisions;

    Tile*** dividedTiles = new Tile**[dividedGridLength];    
    float** tilesTraveled = new float*[dividedGridLength];

    for(int p = 0; p < dividedGridLength; p++)
    {
        dividedTiles[p] = new Tile*[dividedGridHeight];
        tilesTraveled[p] = new float[dividedGridHeight];
    }

    for(int k = 0; k < dividedGridLength; k++)
    {
        for(int j = 0; j < dividedGridHeight; j ++)
        {
            int u = j / p_iTileDivisions;
            int v = k / p_iTileDivisions;

            //The new tiles get assigned their coordinate and a grass flag. Whether or not is has a resource
            // is determined by the tile it was divided from. It isn't selectable, and its size is the original
            // tileSize divided by the amount of subdivision.
            dividedTiles[j][k] = new Tile(j, k, 1, !m_tTiles[u][v]->IsWalkableTerrain(), false,
                                          Tile::GetDefaultSize() / p_iTileDivisions);
            tilesTraveled[j][k] = 0;
        }
    }
    //The new final tile is the center subdivided tile within the final tile.
    int finalX = p_tEndTile.GetGridX() * p_iTileDivisions + (p_iTileDivisions / 2);
    int finalY = p_tEndTile.GetGridY() * p_iTileDivisions + (p_iTileDivisions / 2);

    m_lCurrentList.push_back(Tile(finalX, finalY));
    tilesTraveled[finalX][finalY] = 1;

    //Same goes for the start tile
    int startX = p_tStartTile.GetGridX() * p_iTileDivisions + (p_iTileDivisions / 2);
    int startY = p_tStartTile.GetGridY() * p_iTileDivisions + (p_iTileDivisions / 2);

    Tile startTile = Tile(startX, startY);

    //We haven't found the start and we only took our first step at the village
    bool startFound = false;

    while(!startFound)
    {
        //We create an iterator for the current list
        list<Tile>::iterator it;

        //And booleans for looking at each adjacent tile
        bool startIsUp = false;
        bool startIsLeft = false;
        bool startIsRight = false;
        bool startIsDown = false;

        //For every tile on the current list
        for(it = m_lCurrentList.begin(); it != m_lCurrentList.end() && !startFound; it++)
        {
            Tile currentTile = *it;
            int xPos = currentTile.GetGridX();
            int yPos = currentTile.GetGridY();

            //We check if every adjacent tile to see if it's the start. Since the method doesn't know which
            // direction we came from, we just give it the amount of steps so that it knows how many steps
            // it took to get there.
            if(currentTile.GetGridY() < (dividedGridHeight - 1))
            {
                float steps = tilesTraveled[xPos][yPos] + GetWeightBetweenTiles(currentTile,
                                                              *dividedTiles[xPos][yPos + 1]);
                startIsUp = CheckPlayerTile(*dividedTiles[xPos][yPos + 1],
                                      startTile, tilesTraveled, dividedTiles, steps);
            }

            if(currentTile.GetGridX() > 0)
            {
                float steps = tilesTraveled[xPos][yPos] + GetWeightBetweenTiles(currentTile,
                                                              *dividedTiles[xPos - 1][yPos]);
                startIsLeft = CheckPlayerTile(*dividedTiles[xPos - 1][yPos],
                                        startTile, tilesTraveled, dividedTiles, steps);
            }

            if(currentTile.GetGridX() < (dividedGridHeight - 1))
            {
                float steps = tilesTraveled[xPos][yPos] + GetWeightBetweenTiles(currentTile,
                                                              *dividedTiles[xPos + 1][yPos]);
                startIsRight = CheckPlayerTile(*dividedTiles[xPos + 1][yPos],
                                         startTile, tilesTraveled, dividedTiles, steps);
            }

            if(currentTile.GetGridY() > 0)
            {
                float steps = tilesTraveled[xPos][yPos] + GetWeightBetweenTiles(currentTile,
                                                              *dividedTiles[xPos][yPos - 1]);
                startIsDown = CheckPlayerTile(*dividedTiles[xPos][yPos - 1],
                                        startTile, tilesTraveled, dividedTiles, steps);
            }

            //If any of those m_tTiles we checked is the starting tile, then we've created our path and we exit
            // the loop
            if(startIsUp || startIsLeft || startIsRight || startIsDown)
            {
                startFound = true;
            }
        }

        if(m_lCurrentList.empty() && m_lBufferList.empty())
        {
            return list<Ogre::Vector3>();
        }

        //Whether or not we found the start of our path, we 
        m_lCurrentList.clear();
        m_lCurrentList = list<Tile>(m_lBufferList);
        m_lBufferList.clear();

       /*std::ofstream myfile("tileSheet.txt");
       
       
       for (int i = 0; i < dividedGridHeight; i++)
       {
           for (int j = 0; j < dividedGridLength; j++)
           {
               if(tilesTraveled[j][i] == 0)
                    myfile << " 00.000" << tilesTraveled[j][i];
               else
                    myfile << " " << tilesTraveled[j][i];
           }
           myfile << endl;
       }
       myfile.close();
       */


    }

    //We clear our current list for the next time we pass through our path finding loop.
    m_lCurrentList.clear();

    //We create our queue, which is the path the enemies will follow. We hand it the starting tile.
    list<Tile> shortestPath;

    //We create ints that hold the coordinates of our starting tile. This will determine where we
    // are standing as we hand m_tTiles to the queue.
    int currentX = startTile.GetGridX();
    int currentY = startTile.GetGridY();

    shortestPath.push_back(*dividedTiles[currentX][currentY]);

    float steps = tilesTraveled[currentX][currentY];

    //While the village tile has not been reached.
    while(steps != 1)
    {
       
        //DIRECTIONS
        //0: Up
        //1: Right
        //2: Down
        //3: Left
        
        //We keep track of the shortest direction. -1 means a null direction
        int shortestDirection = -1;

        //The biggest change corresponds to the adjacent tile with the lowest associated
        // tile traversal number.
        float biggestChange = 0;

        //If our location is valid
        if(currentX > 0)
        {
            //If the tile was visited
            if(tilesTraveled[currentX - 1][currentY] != 0)
            {
                float change = steps - tilesTraveled[currentX - 1][currentY];

                //If this adjacent tile was reached in the shortest amount of steps
                if(change > biggestChange)
                {
                    //We set this tile to be the biggest change and set our direction
                    // to the corresponding int
                    biggestChange = change;
                    shortestDirection = 3;
                }
            }
        }
        if(currentX < (dividedGridLength - 1))
        {
            if(tilesTraveled[currentX + 1][currentY] != 0)
            {
                float change = steps - tilesTraveled[currentX + 1][currentY];

                if(change > biggestChange)
                {
                    biggestChange = change;
                    shortestDirection = 1;
                }
            }
        }
        if(currentY > 0)
        {
            if(tilesTraveled[currentX][currentY - 1] != 0)
            {
                float change = steps - tilesTraveled[currentX][currentY - 1];

                if(change > biggestChange)
                {
                    biggestChange = change;
                    shortestDirection = 0;
                }
            }
        }
        if(currentY < (dividedGridHeight - 1))
        {
            if(tilesTraveled[currentX][currentY + 1] != 0)
            {
                float change = steps - tilesTraveled[currentX][currentY + 1];

                if(change > biggestChange)
                {
                    shortestDirection = 2;
                }
            }
        }
    
        //We move ourselves corresponding to which direction we're supposed to go in
        if(shortestDirection == 0)
        {
            currentY--;
        }
        else if(shortestDirection == 1)
        {
            currentX++;
        }
        else if(shortestDirection == 2)
        {
            currentY++;
        }
        else if(shortestDirection == 3)
        {
            currentX--;
        }

        //And we push back that tile
        shortestPath.push_back(*dividedTiles[currentX][currentY]);
        steps = tilesTraveled[currentX][currentY];
    }

    list<Ogre::Vector3> finalList;

    list<Tile>::iterator it;

    //The list of Tiles is translated into a list of world coordinates
    for(it = shortestPath.begin(); it != shortestPath.end(); it++)
    {
        finalList.push_back(it->GetLocation());
    }

    //The path is returned
    return finalList;
}

void TileSet::Destroy()
{
	for (int i = 0; i < m_iGridLength; i++)
	{
        //for (int j = 0; j < m_iGridHeight; j++)
		//{
        //    delete m_tTiles[i][j];
		//}

		delete[] m_tTiles[i];
	}

	delete[] m_tTiles;

	m_lShortestPath.clear();
	m_lBufferList.clear();
	m_lCurrentList.clear();
}