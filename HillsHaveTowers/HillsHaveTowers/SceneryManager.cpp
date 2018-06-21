#include "SceneryManager.h"

#include <sstream>
#include "TileSet.h"

SceneryManager* SceneryManager::m_smInstance;

SceneryManager* SceneryManager::Instance()
{
    if(m_smInstance == NULL)
    {
        m_smInstance = new SceneryManager();
    }

    return m_smInstance;
}

void SceneryManager::AddBridge(Tile p_tTile, bool p_bRotated)
{
    std::ostringstream toString;
    
    toString << m_vModels.size();

    std::string modelName = "Bridge";
    modelName.append(toString.str());

    Model* tempModel = new Model("Bridge_Stone.mesh", modelName);
    
    tempModel->SetQueryFlag(QF_TERRAIN);

    tempModel->Scale(Ogre::Vector3(1.8f, 5.0f, 6.0f));

    Ogre::Vector3 position = p_tTile.GetLocation();
    position.y += TileSet::GetPathHeight() - 9;

    tempModel->TranslateTo(position);

    if(p_bRotated)
    {
        tempModel->Rotate(90);
    }

    m_vModels.push_back(tempModel);
}

void SceneryManager::DeleteAll()
{
    for(int j = 0; j < m_vModels.size(); j++)
    {
        m_vModels[j]->CleanUp();
    }

    m_vModels.clear();
}