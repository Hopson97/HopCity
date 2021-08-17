#include "ScreenMainMenu.h"
#include "ScreenGame.h"
#include <imgui_sfml/imgui.h>

ScreenMainMenu::ScreenMainMenu(ScreenManager* stack)
    : Screen(stack)

{
}

void ScreenMainMenu::onGUI(nk_context* nuklear)
{
    if (imguiBeginMenu("M A I N   M E N U")) {
        ImGui::Text("Welcome to xyz by Hopson");
        ImGui::Separator();

        if (imguiButtonCustom("Start Game")) {
            m_pScreens->pushScreen(std::make_unique<ScreenGame>(m_pScreens));
        }
        if (imguiButtonCustom("Exit Game")) {
            m_pScreens->popScreen();
        }
    }
    ImGui::End();
}
void ScreenMainMenu::onRender(sf::RenderWindow* window) {}
