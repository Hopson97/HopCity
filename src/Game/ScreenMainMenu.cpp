#include "ScreenMainMenu.h"
#include "ScreenGame.h"
#include <imgui_sfml/imgui.h>

ScreenMainMenu::ScreenMainMenu(ScreenManager* stack)
    : Screen(stack)
    , newgameanim(32, 64, false)
    , loopedani(32, 64, false)
{
    for (int i = 0; i < 9; i++) {
        newgameanim.addFrame(0, i, sf::milliseconds(150));
    }
    for (int i = 9; i < 16; i++) {
        loopedani.addFrame(0, i, sf::milliseconds(150));
    }

    testTexutre.loadFromFile("data/Textures/NewGame.png");
    testFrame.setTexture(&testTexutre);
    testFrame.setSize({256, 512});
    testFrame.setPosition(200, 450 - 256);
}

void ScreenMainMenu::onGUI()
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
void ScreenMainMenu::onRender(sf::RenderWindow* window) 
{
    if (!newgameanim.isOnLastFrame()) {
        testFrame.setTextureRect(newgameanim.getFrame());
    }
    else {
        testFrame.setTextureRect(loopedani.getFrame());
    }
    window->draw(testFrame); }
