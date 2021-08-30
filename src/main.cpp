#include "Game/ScreenGame.h"
#include "Game/ScreenMainMenu.h"
#include "Game/WorldGeneration.h"
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
    window.setFramerateLimit(100);
    window.setKeyRepeatEnabled(false);
    ImGui::SFML::Init(window);

    Profiler profiler;

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
    screens.pushScreen(std::make_unique<ScreenMainMenu>(&screens, window));
    screens.update();

    // Time step, 30 ticks per second
    const sf::Time timePerUpdate = sf::seconds(1.0f / 30.0f);
    sf::Clock timer;
    sf::Time lastTime = sf::Time::Zero;
    sf::Time lag = sf::Time::Zero;

    // Misc/ Util
    Keyboard keyboard;
    sf::Clock updateClock;
    bool profilerOpen = false;

    while (window.isOpen() && !screens.isEmpty()) {
        Screen* screen = &screens.peekScreen();
        profiler.reset();

        {

            TimeSlot& profilerSlot = profiler.newTimeslot("Events");

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
            profilerSlot.stop();
        }

        // Get times
        sf::Time dt = updateClock.restart();
        sf::Time time = timer.getElapsedTime();
        sf::Time elapsed = time - lastTime;
        lastTime = time;
        lag += elapsed;

        // User input (real time)
        if (window.hasFocus()) {
            TimeSlot& profilerSlot = profiler.newTimeslot("Input");
            screen->onInput(keyboard, window);
            profilerSlot.stop();
        }

        // Game update
        {
            TimeSlot& profilerSlot = profiler.newTimeslot("Update");
            screen->onUpdate(dt);
            ImGui::SFML::Update(window, dt);
            profilerSlot.stop();
        }

        // Fixed update
        {
            TimeSlot& profilerSlot = profiler.newTimeslot("Fixed Update");
            // Fixed time update
            while (lag >= timePerUpdate) {
                lag -= timePerUpdate;
                screen->onFixedUpdate(elapsed);
            }
            profilerSlot.stop();
        }

        // Rendering
        window.clear({64, 164, 223});
        {
            TimeSlot& profilerSlot = profiler.newTimeslot("Render");
            screen->onRender(&window);
            profilerSlot.stop();
        }

        // GUI/ ImGUI stuff
        {
            TimeSlot& profilerSlot = profiler.newTimeslot("GUI");
            screen->onGUI();
            profilerSlot.stop();
        }

        if (profilerOpen)
            profiler.onGUI();
        ImGui::SFML::Render(window);
        window.display();
        screens.update();
    }
}