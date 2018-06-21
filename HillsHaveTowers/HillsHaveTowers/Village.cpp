//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
//Village.h
//
//PURPOSE:
//Creates a village with animated villagers walking around. Amount of villagers is
// related to how many villagers are left in the village
//
//All code here was written by Nick unless stated otherwise
//~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*

#include "Village.h"
#include "TileSet.h"

Village* Village::m_vVillage;

Village* Village::Instance()
{
    if(m_vVillage == NULL)
    {
        m_vVillage = new Village();
    }

    return m_vVillage;
}

//Don't use the empty constructor
Village::Village()
{
    //Creates a village populated with 20 villagers
    m_iVillagerCount = 20;

    //Sets the timer to 0
    m_fTimeSinceLastFire = 0;

    //Creates 5 houses
    m_mHouses = new Model*[5];

    Ogre::Vector3 tempLoc = TileSet::GetVillageTile()->GetLocation();

    float dist = Tile::GetDefaultSize();

    //Creates 5 tudorhouses, one in the center and then one in each corner of the village
    m_mHouses[0] = new Model("tudorhouse.mesh");
    m_mHouses[0]->TranslateTo(tempLoc + Ogre::Vector3(0, 0, -dist));
    m_mHouses[0]->Translate(Ogre::Vector3(0, 4, 0));
    m_mHouses[0]->Scale(Ogre::Vector3(0.010));

    m_mHouses[1] = new Model("tudorhouse.mesh");
    m_mHouses[1]->TranslateTo(tempLoc + Ogre::Vector3(dist * 0.951f, 0, dist * -0.309f));
    m_mHouses[1]->Translate(Ogre::Vector3(0, 4, 0));
    m_mHouses[1]->Scale(Ogre::Vector3(0.010));
    m_mHouses[1]->Rotate(72);

    m_mHouses[2] = new Model("tudorhouse.mesh");
    m_mHouses[2]->TranslateTo(tempLoc + Ogre::Vector3(dist * 0.587f, 0, dist * 0.809f));
    m_mHouses[2]->Translate(Ogre::Vector3(0, 4, 0));
    m_mHouses[2]->Scale(Ogre::Vector3(0.010));
    m_mHouses[2]->Rotate(144);

    m_mHouses[3] = new Model("tudorhouse.mesh");
    m_mHouses[3]->TranslateTo(tempLoc + Ogre::Vector3(dist * -0.587f, 0, dist * 0.809f));
    m_mHouses[3]->Translate(Ogre::Vector3(0, 4, 0));
    m_mHouses[3]->Scale(Ogre::Vector3(0.010));
    m_mHouses[3]->Rotate(216);

    m_mHouses[4] = new Model("tudorhouse.mesh");
    m_mHouses[4]->TranslateTo(tempLoc + Ogre::Vector3(dist * -0.951f, 0, dist * -0.309f));
    m_mHouses[4]->Translate(Ogre::Vector3(0, 4, 0));
    m_mHouses[4]->Scale(Ogre::Vector3(0.010));
    m_mHouses[4]->Rotate(288);
}

Village::~Village()
{
    delete[] m_mHouses;
    m_mHouses = NULL;

    std::list<Model*>::iterator it;

    for(it = m_lMovingVillagers.begin(); it != m_lMovingVillagers.end(); it++)
    {
        (*it)->CleanUp();
    }

    m_lMovingVillagers.clear();
}

//Removes the specified amount of villagers from the village
int Village::Remove(int p_iCount)
{
    //Subtracts the amount of villagers taken from the village
    m_iVillagerCount -= p_iCount;

    //If the village has negative villagers
    if(m_iVillagerCount < 0)
    {
        //Factor the negative villagers into the villagers taken for the ACTUAL amount of villagers
        // taken. Then set the amount of villagers left to 0
        p_iCount += m_iVillagerCount;
        m_iVillagerCount = 0;
    }

    //return the amount of villagers actually taken
    return p_iCount;
}

void Village::Update(const Ogre::FrameEvent& p_feEvent)
{
    //Add the time since last fire to the timer
    m_fTimeSinceLastFire += p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

    //If the timer hasn't gone off in a while correlated to how many villagers there are
    if(m_fTimeSinceLastFire > (20.0f / m_iVillagerCount))
    {
        //We reset the timer
        m_fTimeSinceLastFire = 0;

        //We create a new animated villager model
        Model* tempModel = new Model("MageMale.mesh");
        tempModel->Scale(Ogre::Vector3(0.20));
        tempModel->PlayAnimation("Walk", true);
        
        //We assign him a random starting house and a random target house
        int startHouse = rand() % 5;
        int endHouse = rand() % 5;

        //If the end house is the same as the starting house then pick another one
        while(startHouse == endHouse)
        {
            endHouse = rand() % 5;
        }

        //We translate the model to the starting house and have him walk towards the target house
        tempModel->TranslateTo(m_mHouses[startHouse]->GetLocation());
        tempModel->TranslateToWithSpeed(Tile::GetDefaultSize() * 0.75f, m_mHouses[endHouse]->GetLocation());
        tempModel->FaceTo(m_mHouses[endHouse]->GetLocation());

        //We push back the model
        m_lMovingVillagers.push_back(tempModel);
    }

    std::list<Model*>::iterator it;

    //If the list isn't empty, we update all of the models
    if(!m_lMovingVillagers.empty())
    {
        for(it = m_lMovingVillagers.begin(); it != m_lMovingVillagers.end(); it++)
        {
            (*it)->Update(p_feEvent);
        }
    }

    std::list<Model*> tempList = m_lMovingVillagers;
    std::list<Model*>::iterator tempIt;

    //If the list isn't empty
    if(!tempList.empty())
    {
        for(it = tempList.begin(); it != tempList.end(); it++)
        {
            //If the model is done translating
            if(!(*it)->IsTranslating())
            {
                //We delete it
                (*it)->CleanUp();
                m_lMovingVillagers.remove(*it);
            }
        }
    }
}



