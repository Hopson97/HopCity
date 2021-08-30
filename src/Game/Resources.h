#pragma once

#include <SFML/Graphics/Texture.hpp>

struct Resources
{
    int coins = 100;
    int food = 100;
    int wood = 100;
    int stone = 100;
    int metal = 100;
    int population = 0;

    Resources sumCost(int num) const;
    bool canAfford(const Resources& other, int n) const;

    Resources operator+=(const Resources& other);
    Resources operator-=(const Resources& other);
    Resources getSellingPrice() const;
};

struct ResourcePanelGUI
{
  public:
    static ResourcePanelGUI& instance();
    void onResourceGUI(const Resources& resources, int cols = 3);

  private:
    ResourcePanelGUI();

    sf::Texture m_texResCoin;
    sf::Texture m_texResMetal;
    sf::Texture m_texResStone;
    sf::Texture m_texResWood;
    sf::Texture m_texResFood;
    sf::Texture m_texResPop;
};