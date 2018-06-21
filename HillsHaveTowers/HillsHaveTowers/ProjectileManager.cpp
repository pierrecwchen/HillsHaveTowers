//-----------------------------------------------------------------------------
// ProjectileManager.cpp                                    By: Keenan Johnston
// 
// Implementation file for ProjectileManager.h
//-----------------------------------------------------------------------------

#include "ProjectileManager.h"

std::vector<Projectile*> ProjectileManager::m_vProjectiles;

//-----------------------------------------------------------------------------
// Create a new enemy with the default parameters for each type
//-----------------------------------------------------------------------------
Projectile* ProjectileManager::Add(ProjectileTypes p_eType, Ogre::Vector3 p_v3Loc, 
                                   float p_fSpeed, BaseEnemy* p_pEnemy, float p_fDamage, 
                                   Ogre::Vector3 p_v3Offset, float p_fRadius, float p_fSlow,
                                   int p_iJump)
{
    if (p_eType == PT_ArrowShot)
        m_vProjectiles.push_back(new ArrowShot(p_v3Loc, p_fSpeed, p_pEnemy, p_fDamage, p_v3Offset));
    else if (p_eType == PT_CannonShot)
        m_vProjectiles.push_back(new Cannonball(p_v3Loc, p_fSpeed, p_pEnemy, p_fDamage, p_fRadius, p_v3Offset));
    else if (p_eType == PT_SlowShot)
        m_vProjectiles.push_back(new Slowball(p_v3Loc, p_fSpeed, p_pEnemy, p_fSlow, p_fRadius, p_v3Offset));
    else if (p_eType == PT_SniperShot)
        m_vProjectiles.push_back(new SniperShot(p_v3Loc, p_fSpeed, p_pEnemy, p_fDamage, p_v3Offset));
    else if (p_eType == PT_LIGHTNING)
        m_vProjectiles.push_back(new LightningBolt(p_v3Loc, p_iJump, p_pEnemy, p_fDamage, p_v3Offset));
    else if (p_eType == PT_MAGIC)
        m_vProjectiles.push_back(new MagicBolt(p_v3Loc, p_fSpeed, p_pEnemy, p_fDamage, p_v3Offset));

    return m_vProjectiles.at(m_vProjectiles.size()-1);
}

//-----------------------------------------------------------------------------
// Delete a projectile from the vector
//-----------------------------------------------------------------------------
void ProjectileManager::Delete(Projectile* p_pProjectile)
{
    // Determine the projectile's position in the vector
    for (int i = 0; i < m_vProjectiles.size(); i++)
    {
        if (p_pProjectile == m_vProjectiles.at(i))
        {
            m_vProjectiles.at(i)->DestroyModel();
            m_vProjectiles.erase(m_vProjectiles.begin()+i);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Create a new enemy with the default parameters for each type
//-----------------------------------------------------------------------------
void ProjectileManager::DeleteAll()
{
    for (int i = 0; i < m_vProjectiles.size(); i++)
        m_vProjectiles.at(i)->DestroyModel();
        
    m_vProjectiles.clear();
}

//-----------------------------------------------------------------------------
// Update all projectiles in the vector and remove any flagged for deletion
//-----------------------------------------------------------------------------
void ProjectileManager::Update(const Ogre::FrameEvent& p_feEvent)
{
    for (int i = 0; i < m_vProjectiles.size(); i++)
    {
        if (m_vProjectiles.at(i)->CanDelete())
        {
            m_vProjectiles.at(i)->DestroyModel();
            m_vProjectiles.erase(m_vProjectiles.begin()+i);
        }
        else
            m_vProjectiles.at(i)->Update(p_feEvent);
    }
}