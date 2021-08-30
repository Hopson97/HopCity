#include "Construction.h"

#include <imgui_sfml/imgui-SFML.h>
#include <imgui_sfml/imgui.h>
#include <unordered_set>

bool structureButton(StructureType structureType, ActionManager& currCon)
{
    const StructureDef& def = StructureRegistry::instance().getStructure(structureType);
    if (ImGui::ImageButton(def.guiTexture, {100, 100})) {
        currCon.strType = structureType;
        currCon.action = ActionManager::Action::Constructing;
    }

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", def.name.c_str());
        ImGui::Text("Cost");
        ResourcePanelGUI::instance().onResourceGUI(def.cost, 1);
        ImGui::EndTooltip();
    }
}

void onConstructionGUI(ActionManager& currCon)
{

    if (ImGui::Button("Sell Objects")) {
        currCon.action = ActionManager::Action::Selling;
    }
    ImGui::Separator();

    if (ImGui::BeginTabBar("construction")) {
        if (ImGui::BeginTabItem("Buildings")) {
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Defence")) {
            structureButton(StructureType::WoodWall, currCon);
            ImGui::SameLine();
            structureButton(StructureType::MudWall, currCon);
            structureButton(StructureType::StoneWall, currCon);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Units")) {
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void forEachLSection(const sf::Vector2i& start, const sf::Vector2i& mid, const sf::Vector2i& end,
                     std::function<void(const sf::Vector2i& tile)> f)
{

    // Hack to prevent the functions running for a tile position more than once
    std::unordered_set<sf::Vector2i, Vec2hash> unique;
    auto tryTile = [&](const sf::Vector2i pos) {
        if (!unique.count(pos)) {
            unique.emplace(pos);
            f(pos);
        }
    };

    if (xDistGreater(start, mid)) {
        int startX = std::min(start.x, mid.x);
        int startY = std::min(mid.y, end.y);

        int endX = std::max(start.x + 1, mid.x + 1);
        int endY = std::max(mid.y + 1, end.y + 1);

        for (int x = startX; x < endX; x++) {
            tryTile({x, start.y});
        }
        for (int y = startY; y < endY; y++) {
            tryTile({mid.x, y});
        }
    }
    else {
        int startX = std::min(mid.x, end.x);
        int startY = std::min(start.y, mid.y);
        int endX = std::max(mid.x + 1, end.x + 1);
        int endY = std::max(start.y + 1, mid.y + 1);

        for (int x = startX; x < endX; x++) {
            tryTile({x, mid.y});
        }
        for (int y = startY; y < endY; y++) {
            tryTile({start.x, y});
        }
    }
}

void forEachQuadSection(const sf::Vector2i& start, const sf::Vector2i& end,
                        std::function<void(const sf::Vector2i& tile)> f)
{
    int startX = std::min(start.x, end.x);
    int startY = std::min(start.y, end.y);
    int endX = std::max(start.x + 1, end.x + 1);
    int endY = std::max(start.y + 1, end.y + 1);

    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            f({x, y});
        }
    }
}