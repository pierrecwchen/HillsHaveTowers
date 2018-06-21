#ifndef SCENERY_MANAGER_H
#define SCENERY_MANAGER_H

#include <vector>
#include "Model.h"
#include "Tile.h"

class SceneryManager
{
public:
    static SceneryManager* Instance();

    void AddBridge(Tile p_tTile, bool p_bRotated);

    void DeleteAll();
private:
    SceneryManager() {}
    ~SceneryManager() {}

    static SceneryManager* m_smInstance;

    std::vector<Model*> m_vModels;
};

#endif