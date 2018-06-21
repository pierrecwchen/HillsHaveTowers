//-----------------------------------------------------------------------------
// MainCurrency.cpp                                         By: Keenan Johnston
// 
// Implementation file for MainCurrency.h
//-----------------------------------------------------------------------------

#include "MainCurrency.h"
#include "TowerMenu.h"
#include "TileMenu.h"

int MainCurrency::m_iGold = 0;
int MainCurrency::m_iWood = 0;
int MainCurrency::m_iStone = 0;
int MainCurrency::m_iIron = 0;
float MainCurrency::m_fPercentage = 0.5f;

//-----------------------------------------------------------------------------
// Sets the currency to the given values for each field.  This is used to
// allocate resources at the start of the game and also for debugging
//-----------------------------------------------------------------------------
void MainCurrency::SetCurrency(int p_iGold, int p_iWood, int p_iStone, int p_iIron)
{
    m_iGold = p_iGold;
    m_iWood = p_iWood;
    m_iStone = p_iIron;
    m_iIron = p_iIron;

    InGameHUD::GetSingletonPtr()->SetCoinStats(m_iGold);
    InGameHUD::GetSingletonPtr()->SetWoodStats(m_iWood);
    InGameHUD::GetSingletonPtr()->SetStoneStats(m_iStone);
    InGameHUD::GetSingletonPtr()->SetIronStats(m_iIron);
	TowerMenu::GetSingletonPtr()->EnableUpgradeButton();
	TileMenu::GetSingletonPtr()->EnableButton();
}
//-----------------------------------------------------------------------------
// Given a currency object, determine if it can be subtracted from this
// currency without any resources falling below 0
//-----------------------------------------------------------------------------
bool MainCurrency::CanAfford(Currency p_rResource)
{
    if ((m_iGold - p_rResource.GetGold()) < 0)
        return false;
    else if ((m_iWood - p_rResource.GetWood()) < 0)
        return false;
    else if ((m_iStone - p_rResource.GetStone()) < 0)
        return false;
    else if ((m_iIron - p_rResource.GetIron()) < 0)
        return false;
    else
        return true;
}
//-----------------------------------------------------------------------------
// Add a currency object to the main currency.  Used for awarding loot from
// enemies mostly
//-----------------------------------------------------------------------------
void MainCurrency::Add(Currency p_rResource)
{ 
    m_iGold += p_rResource.GetGold();
    m_iWood += p_rResource.GetWood();
    m_iStone += p_rResource.GetStone();
    m_iIron += p_rResource.GetIron();

    InGameHUD::GetSingletonPtr()->SetCoinStats(m_iGold);
    InGameHUD::GetSingletonPtr()->SetWoodStats(m_iWood);
    InGameHUD::GetSingletonPtr()->SetStoneStats(m_iStone);
    InGameHUD::GetSingletonPtr()->SetIronStats(m_iIron);
	TowerMenu::GetSingletonPtr()->EnableUpgradeButton();
	TileMenu::GetSingletonPtr()->EnableButton();
}
//-----------------------------------------------------------------------------
// Subtract the given amount of currency from the main currency.  Generally
// used after a CanAfford call returning true for building a tower
//-----------------------------------------------------------------------------
void MainCurrency::Subtract(Currency p_rResource)
{ 
    m_iGold -= p_rResource.GetGold();
    m_iWood -= p_rResource.GetWood();
    m_iStone -= p_rResource.GetStone();
    m_iIron -= p_rResource.GetIron();

    InGameHUD::GetSingletonPtr()->SetCoinStats(m_iGold);
    InGameHUD::GetSingletonPtr()->SetWoodStats(m_iWood);
    InGameHUD::GetSingletonPtr()->SetStoneStats(m_iStone);
    InGameHUD::GetSingletonPtr()->SetIronStats(m_iIron);
	TowerMenu::GetSingletonPtr()->EnableUpgradeButton();
	TileMenu::GetSingletonPtr()->EnableButton();
}
//-----------------------------------------------------------------------------
// Give the player a percentage of the currency object passed in.  This is used
// when selling towers so they don't need to store their sell costs, only build
//-----------------------------------------------------------------------------
void MainCurrency::Refund(Currency p_rResource)
{
    m_iGold += p_rResource.GetGold() * m_fPercentage;
    m_iWood += p_rResource.GetWood() * m_fPercentage;
    m_iStone += p_rResource.GetStone() * m_fPercentage;
    m_iIron += p_rResource.GetIron() * m_fPercentage;

    InGameHUD::GetSingletonPtr()->SetCoinStats(m_iGold);
    InGameHUD::GetSingletonPtr()->SetWoodStats(m_iWood);
    InGameHUD::GetSingletonPtr()->SetStoneStats(m_iStone);
    InGameHUD::GetSingletonPtr()->SetIronStats(m_iIron);
	TowerMenu::GetSingletonPtr()->EnableUpgradeButton();
	TileMenu::GetSingletonPtr()->EnableButton();
}
//-----------------------------------------------------------------------------
// Add the given amount of gold to the main currency
//-----------------------------------------------------------------------------
void MainCurrency::AddGold(int p_iGold)
{ 
    m_iGold += p_iGold; 
    InGameHUD::GetSingletonPtr()->SetCoinStats(m_iGold);
	TowerMenu::GetSingletonPtr()->EnableUpgradeButton();
	TileMenu::GetSingletonPtr()->EnableButton();
}
//-----------------------------------------------------------------------------
// Add the given amount of wood to the main currency
//-----------------------------------------------------------------------------
void MainCurrency::AddWood(int p_iWood) 
{ 
    m_iWood += p_iWood; 
    InGameHUD::GetSingletonPtr()->SetWoodStats(m_iWood);
	TowerMenu::GetSingletonPtr()->EnableUpgradeButton();
	TileMenu::GetSingletonPtr()->EnableButton();
}
//-----------------------------------------------------------------------------
// Add the given amount of stone to the main currency
//-----------------------------------------------------------------------------
void MainCurrency::AddStone(int p_iStone) 
{ 
    m_iStone += p_iStone;
    InGameHUD::GetSingletonPtr()->SetStoneStats(m_iStone);
	TowerMenu::GetSingletonPtr()->EnableUpgradeButton();
	TileMenu::GetSingletonPtr()->EnableButton();
}
//-----------------------------------------------------------------------------
// Add the given amount of iron to the main currency
//-----------------------------------------------------------------------------
void MainCurrency::AddIron(int p_iIron) 
{
    m_iIron += p_iIron; 
    InGameHUD::GetSingletonPtr()->SetIronStats(m_iIron);
	TowerMenu::GetSingletonPtr()->EnableUpgradeButton();
	TileMenu::GetSingletonPtr()->EnableButton();
}