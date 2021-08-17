#include "Game/ScreenGame.h"
#include "Game/ScreenMainMenu.h"
#include "Game/World.h"
#include "Keyboard.h"
#include "Profiler.h"
#include "Screen.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui_sfml/imgui-SFML.h>
#include <imgui_sfml/imgui.h>

int main()
{
    // Set up window and gui
    sf::RenderWindow window({1600, 900}, "game");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    ImGui::SFML::Init(window);

    // Update ImGUI UI scaling for 4K monitors. For some reason looks bad on linux hence
    // the ifdef
#ifndef __linux__
    if (sf::VideoMode::getDesktopMode().width > 3600) {
        ImGui::GetIO().FontGlobalScale = ((window.getSize().x) / 1024.f);
    }
#endif
    // Set up screen system
    ScreenManager screens;
    // screens.pushScreen(std::make_unique<ScreenMainMenu>(&screens));
    screens.pushScreen(std::make_unique<ScreenMainMenu>(&screens));
    screens.update();

    // Time step, 30 ticks per second
    const sf::Time timePerUpdate = sf::seconds(1.0f / 30.0f);
    sf::Clock timer;
    sf::Time lastTime = sf::Time::Zero;
    sf::Time lag = sf::Time::Zero;

    // Misc/ Util
    Keyboard keyboard;
    sf::Clock updateClock;

    while (window.isOpen() && !screens.isEmpty()) {
        Screen* screen = &screens.peekScreen();
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
                    break;

                default:
                    break;
            }
        }

        // Get times
        sf::Time dt = updateClock.restart();
        sf::Time time = timer.getElapsedTime();
        sf::Time elapsed = time - lastTime;
        lastTime = time;
        lag += elapsed;

        // Real time stuff
        screen->profiler.reset();
        screen->onInput(keyboard, window);
        screen->onUpdate(dt);
        ImGui::SFML::Update(window, dt);

        ImGui::ShowDemoWindow();

        // Fixed time update
        while (lag >= timePerUpdate) {
            lag -= timePerUpdate;
            screen->onFixedUpdate(elapsed);
        }

        // Rendering
        window.clear({64, 164, 223});
        screen->onRender(&window);

        screen->onGUI();
        ImGui::SFML::Render(window);

        window.display();
        screens.update();
    }

}