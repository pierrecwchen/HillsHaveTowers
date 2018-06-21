//-----------------------------------------------------------------------------
// Villager.cpp                                             By: Keenan Johnston
// 
// Implementation file for Villager.h
//-----------------------------------------------------------------------------

#include "Villager.h"
#include "Village.h"
#include "SoundManager.h"

Villager::Villager(Ogre::Vector3 p_v3Loc, Tile p_tStartTile, std::list<Tile> p_lShortestPath)
{
	// Set up villager stats
	m_fWalkSpeed = 0.75;

    // Set up path
    m_lCurrentPath = p_lShortestPath;
    m_tCurrentTile = Tile(p_v3Loc);

    if(!IsOnNewPath())
    {
        m_tCurrentTile = p_tStartTile;
    }

    SetShortestPath(m_lCurrentPath);

    m_bIsOnCorrectPath = true;

    m_bCanDelete = false;

    // Create the visual representation of the enemy and play its walk animation
    m_pModel = new Model("MageMale.mesh");

    m_iHeight = Terrain::GetHeight(m_tCurrentTile.GetLocation().x, m_tCurrentTile.GetLocation().z) - 20;
    m_fXOffset = (((float)(rand() % 1001) / 1000) - 0.5f) * Tile::GetDefaultSize() / 1.1;
    m_fZOffset = (((float)(rand() % 1001) / 1000) - 0.5f) * Tile::GetDefaultSize() / 1.1;

    Ogre::Vector3 position = m_lCurrentPath.front().GetLocation();
    position.y += m_iHeight;
    m_pModel->Translate(position);

    m_lCurrentPath.pop_front();
    m_tCurrentTile = m_lCurrentPath.front();

    m_pModel->Rotate(GetAngleToNextTile());
   
    m_pModel->Scale(Ogre::Vector3(0.35));
	m_pModel->PlayAnimation("Walk", true);

    // Default to walk state
    m_eState = eWalkState;
}

Villager::~Villager()
{
    m_pModel->CleanUp();
    delete m_pModel;
}

//-----------------------------------------------------------------------------
// Get a list of the tiles that form the path for movement
//-----------------------------------------------------------------------------
void Villager::SetShortestPath(std::list<Tile> p_lPath)
{
    //We set our shortest path to "the received path" indicating that we're simply holding onto
    // it for now
    m_lReceivedPath = p_lPath;

    //If we're on this newly received path
    if(IsOnNewPath())
    {
        //We'll set a flag indicating that we are indeed on the new path
        m_bIsOnCorrectPath = true;

        //We don't know which tile on the path we're on, we're not matched to one yet, so we set
        // this to false
        bool matchFound = false;

        //If we're heading to the village
        //While we haven't found our tile
        while(!matchFound)
        {
            //We check if this tile is our tile
            if(!(m_tCurrentTile == m_lReceivedPath.front()))
            {
                //If not, toss it
                m_lReceivedPath.pop_front();
            }
            else
            {
                matchFound = true;
            }

            
            //Once we found our tile, we start from there and continue on
            m_lCurrentPath = m_lReceivedPath;
        }
    }
    //If we aren't actually on our new path at the moment, then we flag ourselves as off 
    // the path and we just continue with the old one, saving the pathfinding for later
    else
    {
        m_bIsOnCorrectPath = false;
    }
}
//-----------------------------------------------------------------------------
// Check if the tile the enemy is currently on is part of the new path
//-----------------------------------------------------------------------------
bool Villager::IsOnNewPath()
{
    std::list<Tile>::iterator it;

    for(it = m_lReceivedPath.begin(); it != m_lReceivedPath.end(); it++)
    {
        if(m_tCurrentTile == *it) return true;
    }

    return false;
}
//-----------------------------------------------------------------------------
// Use OGRE to delete model-related data before destroying this object
//-----------------------------------------------------------------------------
void Villager::DestroyModel()
{
	m_lCurrentPath.clear();
	m_lReceivedPath.clear();

    m_pModel->CleanUp();
	delete m_pModel;
}
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
// NICK: Update this object
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
void Villager::Update(const Ogre::FrameEvent& p_feEvent)
{
    //We find the angle between the enemy and the next tile he wants to go to
    float angle = GetAngleToNextTile();

    //We then find what the difference is between the model's current orientation and the angle to the next tile. We also
    // multiply it by the timeSinceLastFrame so that the enemy doesn't turn instantly. We also multiply it by an arbitrary
    // number so that the enemy rotates fast enough.
    float rotation = GetAngleDifference(m_pModel->GetRotation(), angle) * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed() * 2.5 * m_fWalkSpeed;

    //If the rotation isn't redundant
    if(rotation > 0.001 || rotation < -0.001)
    {
        //Rotate the model
        m_pModel->Rotate(rotation);
    }

    //We then move the model forward.
    m_pModel->MoveForward(m_fWalkSpeed * Tile::GetDefaultSize() * p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed());

    //If we reach the end of the path
    if(m_lCurrentPath.size() == 0)
    {
        //We delete ourselves
        Village::Instance()->Add();
        PlayReturnSound();
        m_bCanDelete = true;
    }
    else
    {
        //If we reached our next tile
        if(Tile(m_pModel->GetLocation()) == m_tCurrentTile)
        {
            //We pop it off the list
            m_lCurrentPath.pop_front();

            //If we're not actually on the right path yet, we check if we're on it now
            if(!m_bIsOnCorrectPath)
            {
                SetShortestPath(m_lReceivedPath);
            }
        }

        //If this isn't our last tile
        if(m_lCurrentPath.size() != 0)
        {
            //We get the next target tile
            m_tCurrentTile = m_lCurrentPath.front();
        }
    }

    // Update the model
    m_pModel->Update(p_feEvent);
}

//~*~*~*~*~*~*~*~*~*
//NICK: Refer to BaseEnemy::GetAngleDifference
//~*~*~*~*~*~*~*~*~*
float Villager::GetAngleDifference(float p_fCurrent, float p_fDestination)
{
    if(p_fDestination - p_fCurrent < 180 && p_fDestination - p_fCurrent > -180)
    {
        return p_fDestination - p_fCurrent;
    }
    else if (p_fDestination > 180)
    {
        return p_fDestination - 360 - p_fCurrent;
    }
    else
    {
        return p_fDestination - p_fCurrent + 360;
    }
}

//~*~*~*~*~*~*~*~*~*
//NICK: Refer to BaseEnemy::GetAngleToNextTile
//~*~*~*~*~*~*~*~*~*
float Villager::GetAngleToNextTile()
{
    Ogre::Vector3 destination = Ogre::Vector3(m_tCurrentTile.GetLocation().x + m_fXOffset, 0,
        m_tCurrentTile.GetLocation().z + m_fZOffset);

    Ogre::Vector3 direction = destination - m_pModel->GetLocation();

    float angle;

    if(direction.z < 0)
    {
        float radians = atan(direction.x / direction.z);
        angle = radians / 2 / 3.1415926535 * 360 + 180;
    }
    else if(direction.x < 0)
    {
        float radians = atan(direction.x / direction.z);
        angle = 360 + radians / 2 / 3.1415926535 * 360;
    }
    else if(direction.x > 0)
    {
        float radians = atan(direction.x / direction.z);
        angle = radians / 2 / 3.1415926535 * 360;
    }
    else
    {
        angle = m_pModel->GetRotation();
    }

    return angle;
}

void Villager::PlayReturnSound()
{
    int selection = rand() % 10;

    std::string fileNames[10];

    fileNames[0] = "villagerReturn1.wav";
    fileNames[1] = "villagerReturn2.wav";
    fileNames[2] = "villagerReturn3.wav";
    fileNames[3] = "villagerReturn4.wav";
    fileNames[4] = "villagerReturn5.wav";
    fileNames[5] = "villagerReturn6.wav";
    fileNames[6] = "villagerReturn7.wav";
    fileNames[7] = "villagerReturn8.wav";
    fileNames[8] = "villagerReturn9.wav";
    fileNames[9] = "villagerReturn10.wav";

    SoundManager::Play3DPitchedSound(fileNames[selection], m_pModel->GetLocation(), 0.1f);
}

/**************************************************************************************
*	Handle the events. (Added by Pierre)
***************************************************************************************/
void Villager::HandleEvent(const Hills::GameEvent& p_geGameEvent)
{
	if(p_geGameEvent.GetGameEventType() == Hills::GE_OpenNewPath)
	{
		SetShortestPath(p_geGameEvent.GetShortestPath());
	}
}