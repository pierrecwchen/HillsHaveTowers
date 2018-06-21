#include "MeteorPower.h"
#include "EventManager.h"
#include "BaseApplication.h"
#include "SoundManager.h"
#include "Tile.h"

MeteorPower::MeteorPower() 
{
    m_sotRequired = SO_Tile;

    m_fTimeUntilReady = 0.0f;
    m_fCoolDown = 40.0f;

    m_fTimeUntilDone = 0.0f;
    m_fActiveLength = 5.0f;
}

void MeteorPower::Activate(Selectable* p_sTarget)
{
    Tile* tempTile = dynamic_cast<Tile*>(p_sTarget);

    for(int j = 0; j < 3; j++)
    {
        m_vMeteors.push_back(new Model("Cannonball.mesh"));
        m_vPlayedWooshSound.push_back(false);

        m_vMeteors[j]->Scale(Ogre::Vector3(2));

        float positionX = (float)rand() / (float)RAND_MAX * 2 * Tile::GetDefaultSize() - Tile::GetDefaultSize();
        float positionZ = (float)rand() / (float)RAND_MAX * 2 * Tile::GetDefaultSize() - Tile::GetDefaultSize();

        Ogre::Vector3 topPosition = tempTile->GetLocation() + Ogre::Vector3(positionX, 500 + 200 * j, positionZ);
        Ogre::Vector3 bottomPosition = Ogre::Vector3(topPosition.x, Terrain::GetHeight(topPosition.x, topPosition.z), topPosition.z);

        m_vMeteors[j]->TranslateTo(topPosition);
        m_vMeteors[j]->TranslateToWithSpeed(200, bottomPosition);
    }

    m_bActive = true;

    m_fTimeUntilDone = m_fActiveLength;
    m_fTimeUntilReady = m_fCoolDown;
}

void MeteorPower::Update(Ogre::FrameEvent p_feEvent)
{
    m_fTimeUntilReady -= p_feEvent.timeSinceLastFrame * BaseApplication::GetGameSpeed();

    if(m_bActive)
    {
        if(m_vMeteors.empty())
        {
            m_bActive = false;
        }
        else
        {
            for(int j = 0; j < m_vMeteors.size();)
            {
                if(!m_vMeteors[j]->IsTranslating())
                {
                    PlayHitSound(m_vMeteors[j]->GetLocation());
                    Hills::EventManager::AddEvent(Hills::GameEvent(Hills::GE_AreaAttack,
                                                  Hills::AOE_Sphere,
                                                  m_vMeteors[j]->GetLocation(),
                                                  Tile::GetDefaultSize() * 2,
                                                  1500,
                                                  0));

                    m_vMeteors[j]->CleanUp();

                    m_vMeteors.erase(m_vMeteors.begin() + j);
                    m_vPlayedWooshSound.erase(m_vPlayedWooshSound.begin() + j);
                }
                else
                {
                    m_vMeteors[j]->Update(p_feEvent);

                    if(!m_vPlayedWooshSound[j] && m_vMeteors[j]->TimeLeftTranslating() < 0.8f)
                    {
                        SoundManager::Play3DSound("meteorWoosh.wav", m_vMeteors[j]->GetLocation(), 0.3f);
                        m_vPlayedWooshSound[j] = true;
                    }

                    j++;
                }
            }
        }
    }
}

void MeteorPower::PlayHitSound(Ogre::Vector3 p_v3Location)
{
    int selection = rand() % 3;

    std::string fileNames[3];

    fileNames[0] = "explode1.wav";
    fileNames[1] = "explode2.wav";
    fileNames[2] = "explode3.wav";

    SoundManager::Play3DSound(fileNames[selection], p_v3Location, 0.5f);
}