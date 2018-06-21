#include "LevelParser.h"
#include "EnemyBar.h"

int** LevelParser::m_iTerrainLayer;
int** LevelParser::m_iResourceLayer;
int LevelParser::m_iTerrainXOffset;
int LevelParser::m_iTerrainZOffset;
float LevelParser::m_fTileSize;
EnemyWaveSender* LevelParser::m_ewsWaves;
std::string LevelParser::m_sHeightMapName;
std::string LevelParser::m_sMaterialName;
int LevelParser::m_iXTiles;
int LevelParser::m_iZTiles;
int LevelParser::m_iTerrainStretchX;

void LevelParser::ReadFile(std::string p_sName)
{
    //-------------------------------
    //Created by: Keenan
    //-------------------------------

    m_iXTiles = 20;
    m_iZTiles = 20;

    //Create the arrays to be filled
    m_iTerrainLayer = new int*[20];
    m_iResourceLayer = new int*[20];

    for (int i = 0; i < 20; i++)
    {
        m_iTerrainLayer[i] = new int[20];
        m_iResourceLayer[i] = new int[20];
    }

    bool doneParsing = false;
    ifstream file;
    string line;
    int pCount = 0;

    file.open(p_sName, std::ios::in);

    while (!doneParsing)
    {
        getline(file, line);

        // if a line is blank skip it
        if (line.length() == 0)
            continue;
        // if a line contains # as the first character, skip it (comment line)
        else if (line.at(0) == '#')
            continue;
        // Read in the terrain layers first
        else
        {
            for (int i = 0; i < ((line.length())/2)+1; i++)
            {
                if (pCount < 20)
                    m_iTerrainLayer[i][pCount%20] = line.at(i*2)-48;
                else
                    m_iResourceLayer[i][pCount%20] = line.at(i*2)-48;
            }
            pCount++;
        }

        if (pCount == 40)
            doneParsing = true;

    }
    //------------------------------------

    doneParsing = false;

    m_ewsWaves = new EnemyWaveSender();

    //Read in the waves
    while(!doneParsing)
    {
        getline(file, line);

        if (line.length() == 0)
            continue;
        // if a line contains # as the first character, skip it (comment line)
        else if (line.at(0) == '#')
            continue;
        //If a line starts with @, it signifies the end of file
        else if (line.at(0) == '@')
            break;

        //Transform the line from a string to a char array
        const char* ccLine = line.c_str();
        char* cLine = new char[line.length() + 1];
        std::strcpy(cLine, ccLine);
        
        EnemyWave* tempWave = new EnemyWave();

        //Read in each section of data. While parameters are separated by commas and sections
        // are separated by semicolons, that doesn't NEED to be the case with this code. It just
        // makes it easier to read.
        char* enemyType = std::strtok(cLine, " ,;");
        char* enemyCount = std::strtok(NULL, " ,;");
        char* enemyDelay = std::strtok(NULL, " ,;");
        char* priestIncluded = std::strtok(NULL, " ,;");

        //While we actually have data
        while(enemyType != NULL)
        {
            //If a % sign is used, this means its the end of wave
            if(enemyType[0] == '%')
                break;

            EnemyGroup* tempGroup;
            
			EnemyBar::GetSingletonPtr()->CreateAnimation(std::atoi(enemyType), std::atoi(enemyCount), std::atof(enemyDelay));

            //We create the a group of the type of enemy
            if(std::atoi(enemyType) == 0)
            {
                tempGroup = new EnemyGroup(SoldierEnemy, std::atoi(enemyCount), std::atof(enemyDelay), std::atoi(priestIncluded));
            }
            else if(std::atoi(enemyType) == 1)
            {
                tempGroup = new EnemyGroup(ScoutEnemy, std::atoi(enemyCount), std::atof(enemyDelay), std::atoi(priestIncluded));
            }
            else if(std::atoi(enemyType) == 2)
            {
                tempGroup = new EnemyGroup(PriestEnemy, std::atoi(enemyCount), std::atof(enemyDelay), std::atoi(priestIncluded));
            }
            else 
            {
                tempGroup = new EnemyGroup(KnightEnemy, std::atoi(enemyCount), std::atof(enemyDelay), std::atoi(priestIncluded));
            }

            //And we add that group to the wave
            tempWave->Add(tempGroup);

            enemyType = std::strtok(NULL, " ,;");
            enemyCount = std::strtok(NULL, " ,;");
            enemyDelay = std::strtok(NULL, " ,;");
            priestIncluded = std::strtok(NULL, " ,;");
        }

        //Once all groups are added to the wave, we add that wave to the wave manager
        m_ewsWaves->Add(tempWave);
    }
}

void LevelParser::ReadLevelFile(std::string p_sName)
{
    tinyxml2::XMLDocument levelFile;

    levelFile.LoadFile(p_sName.c_str());  

    tinyxml2::XMLElement* level = levelFile.FirstChildElement("Level");

    m_sHeightMapName = std::string(level->FirstChildElement("HeightMap")->GetText());
    m_sMaterialName = std::string(level->FirstChildElement("Material")->GetText());
    
    level->FirstChildElement("TerrainXOffset")->QueryIntText(&m_iTerrainXOffset);
    level->FirstChildElement("TerrainZOffset")->QueryIntText(&m_iTerrainZOffset);
    level->FirstChildElement("TileSize")->QueryFloatText(&m_fTileSize);

    level->FirstChildElement("XStretch")->QueryIntText(&m_iTerrainStretchX);

    Tile::SetXOffset(m_iTerrainXOffset);
    Tile::SetZOffset(m_iTerrainZOffset);
    Tile::SetDefaultSize(m_fTileSize);

    m_iZTiles = GetNumElements(level->FirstChildElement("TileLayout"), "row");
    m_iXTiles = GetTerrainWidth(level->FirstChildElement("TileLayout"));

    m_iTerrainLayer = new int*[m_iXTiles];
    m_iResourceLayer = new int*[m_iXTiles];

    for (int i = 0; i < m_iXTiles; i++)
    {
        m_iTerrainLayer[i] = new int[m_iZTiles];
        m_iResourceLayer[i] = new int[m_iZTiles];
    }

    tinyxml2::XMLElement* terrainRow = level->FirstChildElement("TileLayout")->FirstChildElement("row");

    std::string line;

    for(int k = 0; k < m_iZTiles; k++)
    {
        line = std::string(terrainRow->GetText());

        for(int j = 0; j < m_iXTiles; j++)
        {
            m_iTerrainLayer[j][k] = line.at(j * 2) - 48;
        }

        terrainRow = terrainRow->NextSiblingElement("row");
    }

    tinyxml2::XMLElement* resourceRow = level->FirstChildElement("ResourceLayout")->FirstChildElement("row");

    for(int k = 0; k < m_iZTiles; k++)
    {
        line = std::string(resourceRow->GetText());

        for(int j = 0; j < m_iXTiles; j++)
        {
            m_iResourceLayer[j][k] = line.at(j * 2) - 48;
        }
        
        resourceRow = resourceRow->NextSiblingElement("row");
    }

    ReadWaveFile(std::string(level->FirstChildElement("EnemyWaves")->GetText()));
}

void LevelParser::ReadWaveFile(std::string p_sName)
{
    tinyxml2::XMLDocument waveFile;
    std::string finalName = "../Media/levels/";

    finalName.append(p_sName);

    waveFile.LoadFile(finalName.c_str());
    tinyxml2::XMLElement* enemyWaves = waveFile.FirstChildElement("EnemyWaves");

    int numWaves = GetNumElements(enemyWaves, "Wave");
    tinyxml2::XMLElement* nextWave = enemyWaves->FirstChildElement("Wave");

    m_ewsWaves = new EnemyWaveSender();

    for(int j = 0; j < numWaves; j++)
    {
        int numGroups = GetNumElements(nextWave, "Group");
        tinyxml2::XMLElement* nextGroup = nextWave->FirstChildElement("Group");

        int lastEnemyEntry = 0;

        EnemyWave* tempWave = new EnemyWave(nextWave->BoolAttribute("night"));

        for(int k = 0; k < numGroups; k++)
        {
            int enemyType;
            int enemyCount;
            float enemyDelay;
            bool hasPriest;

            nextGroup->QueryIntAttribute("type", &enemyType);
            nextGroup->QueryIntAttribute("count", &enemyCount);
            nextGroup->QueryFloatAttribute("delay", &enemyDelay);
            nextGroup->QueryBoolAttribute("hasPriest", &hasPriest);

            EnemyGroup* tempGroup;

            EnemyBar::GetSingletonPtr()->CreateAnimation(enemyType, enemyCount, enemyDelay/* - lastEnemyEntry*/);

            if(enemyType == 0)
            {
                tempGroup = new EnemyGroup(SoldierEnemy, enemyCount, enemyDelay - lastEnemyEntry, hasPriest);
            }
            else if(enemyType == 1)
            {
                tempGroup = new EnemyGroup(ScoutEnemy, enemyCount, enemyDelay - lastEnemyEntry, hasPriest);
            }
            else if(enemyType == 2)
            {
                tempGroup = new EnemyGroup(PriestEnemy, enemyCount, enemyDelay - lastEnemyEntry, hasPriest);
            }
            else if(enemyType == 3)
            {
                tempGroup = new EnemyGroup(KnightEnemy, enemyCount, enemyDelay - lastEnemyEntry, hasPriest);
            }
            else
            {
                tempGroup = new EnemyGroup(ET_DEMON, enemyCount, enemyDelay - lastEnemyEntry, hasPriest);
            }

            tempWave->Add(tempGroup);

            lastEnemyEntry = enemyDelay;

            nextGroup = nextGroup->NextSiblingElement("Group");
        }

        m_ewsWaves->Add(tempWave);
        nextWave = nextWave->NextSiblingElement("Wave");
    }
}

int LevelParser::GetTerrainWidth(tinyxml2::XMLElement* p_xElement)
{
    std::string temp(p_xElement->FirstChildElement("row")->GetText());

    return temp.size() / 2 + 1;
}

int LevelParser::GetNumElements(tinyxml2::XMLElement* p_xElement, const char* p_cName)
{
    tinyxml2::XMLElement* temp = p_xElement->FirstChildElement(p_cName);

    int rowCount = 1;

    while(temp != p_xElement->LastChildElement(p_cName))
    {
        rowCount++;
        temp = temp->NextSiblingElement(p_cName);
    }

    return rowCount;
}

void LevelParser::Destroy()
{
	for(int i = 0; i < m_iXTiles; i++)
	{
		delete[] m_iTerrainLayer[i];
		delete[] m_iResourceLayer[i];
	}
	delete[] m_iTerrainLayer;
	delete[] m_iResourceLayer;
	delete m_ewsWaves;
}