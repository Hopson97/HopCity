#include "Resources.h"

#include <imgui_sfml/imgui-SFML.h>
#include <imgui_sfml/imgui.h>

namespace
{
    void drawResourceLine(const sf::Texture& texture, int value, const char* tooltip)
    {
        ImGui::Image(texture, {32, 32});
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("%s", tooltip);
            ImGui::EndTooltip();
        }
        ImGui::AlignTextToFramePadding();
        ImGui::SameLine();
        ImGui::Text("%d", value);
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("%s", tooltip);
            ImGui::EndTooltip();
        }
    }
} // namespace

ResourcePanelGUI::ResourcePanelGUI()
{
    m_texResCoin.loadFromFile("data/Textures/GUI/ResCoin.png");
    m_texResMetal.loadFromFile("data/Textures/GUI/ResMetal.png");
    m_texResStone.loadFromFile("data/Textures/GUI/ResStone.png");
    m_texResWood.loadFromFile("data/Textures/GUI/ResWood.png");
    m_texResFood.loadFromFile("data/Textures/GUI/ResFood.png");
    m_texResPop.loadFromFile("data/Textures/GUI/ResPopulation.png");
}

void ResourcePanelGUI::onResourceGUI(const Resources& resources, int cols)
{
    ImGui::Columns(cols, nullptr, false);

    drawResourceLine(m_texResCoin, resources.coins, "Coins");
    drawResourceLine(m_texResFood, resources.food, "Food");
    ImGui::NextColumn();

    drawResourceLine(m_texResWood, resources.wood, "Wood");

    drawResourceLine(m_texResStone, resources.stone, "Stone");

    ImGui::NextColumn();

    drawResourceLine(m_texResMetal, resources.metal, "Metal");

    drawResourceLine(m_texResPop, resources.population, "Population");
    ImGui::Columns(1);
}

ResourcePanelGUI& ResourcePanelGUI::instance()
{
    static ResourcePanelGUI gui;
    return gui;
}

Resources Resources::sumCost(int num) const
{
    return Resources{
        coins * num, food * num, wood * num, stone * num, metal * num, population * num,
    };
}

bool Resources::canAfford(const Resources& other, int n) const
{
    auto cost = other.sumCost(n);
    return cost.food <= food && cost.metal <= metal && cost.wood <= wood && cost.stone <= stone &&
           cost.coins <= coins && cost.population <= population;
}

Resources Resources::operator-=(const Resources& other)
{
    food -= other.food;
    metal -= other.metal;
    wood -= other.wood;
    stone -= other.stone;
    coins -= other.coins;
    population -= other.population;
}

Resources Resources::getSellingPrice() const
{
    Resources res = *this;
    res.food /= 2;
    res.metal /= 2;
    res.wood /= 2;
    res.stone /= 2;
    res.coins /= 2;
    res.population /= 2;
    return res;
}

Resources Resources::operator+=(const Resources& other)
{
    food += other.food;
    metal += other.metal;
    wood += other.wood;
    stone += other.stone;
    coins += other.coins;
    population += other.population;
}
