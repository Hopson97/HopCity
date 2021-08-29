#include "Construction.h"

#include "Structures.h"
#include <imgui_sfml/imgui-SFML.h>
#include <imgui_sfml/imgui.h>

bool structureButton(StructureType structureType, CurrentConstruction& currCon)
{
    const StructureDef& def = StructureRegistry::instance().getStructure(structureType);
    bool res = ImGui::ImageButton(def.guiTexture, {100, 100});
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s", def.name.c_str());
        ImGui::EndTooltip();
    }

    if (res) {
        currCon.strType = structureType;
    }
    return res;
}

void onConstructionGUI(CurrentConstruction& currCon)
{
    float panelSize = 256;
    ImGui::SetNextWindowBgAlpha(1);
    ImGui::SetNextWindowPos({1600 - panelSize, 0});
    ImGui::SetNextWindowSize({panelSize, 900});
    if (ImGui::Begin("Panel", nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Separator();
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
        ImGui::End();
    }
}
