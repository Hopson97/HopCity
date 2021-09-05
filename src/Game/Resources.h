#pragma once

#include <SFML/Graphics/Texture.hpp>

struct Resources {
    int coins = 0;
    int food = 0;
    int wood = 0;
    int stone = 0;
    int metal = 0;
    int population = 0;

    bool canAfford(const Resources& other) const;

    Resources getSellingPrice() const;

    Resources operator+=(const Resources& other)
    {
        food += other.food;
        metal += other.metal;
        wood += other.wood;
        stone += other.stone;
        coins += other.coins;
        population += other.population;
        return *this;
    }

    Resources operator-=(const Resources& other)
    {
        food -= other.food;
        metal -= other.metal;
        wood -= other.wood;
        stone -= other.stone;
        coins -= other.coins;
        population -= other.population;
        return *this;
    }
};

inline Resources operator*(const Resources& left, int num)
{
    Resources res{0};
    res.food = left.food * num;
    res.metal = left.metal * num;
    res.wood = left.wood * num;
    res.stone = left.stone * num;
    res.coins = left.coins * num;
    res.population = left.population * num;
    return res;
}

struct ResourcePanelGUI {
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