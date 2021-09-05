#pragma once
#include "ScreenGame.h"
#include "ScreenMainMenu.h"
#include "WorldGeneration.h"
#include "../Keyboard.h"
#include "../Profiler.h"
#include "../Screen.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui_sfml/imgui-SFML.h>
#include <imgui_sfml/imgui.h>


void ProcessEvents(sf::Window& window, Screen* screen, Keyboard& keyboard,bool& profilerOpen){
    sf::Event e;
    while (window.pollEvent(e)) {
        ImGui::SFML::ProcessEvent(e);
        screen->onEvent(e);
        keyboard.update(e);
        switch (e.type) {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyReleased:
                if (e.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                if (e.key.code == sf::Keyboard::F3)
                    profilerOpen = !profilerOpen;
                break;

            default:
                break;
        }
}
}
