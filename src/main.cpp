#include "Keyboard.h"
#include "Screen.h"
#include "ScreenMainMenu.h"
#include <SFML/Graphics/RenderWindow.hpp>

#include <imgui_sfml/imgui-SFML.h>
#include <imgui_sfml/imgui.h>
#include "Game/World.h"

int main()
{
    //sf::Image grass;
    //sf::Image path;
    //sf::Image target;
//
    //grass.loadFromFile("Data/Tiles/Grass.png");
    //path.loadFromFile("Data/Tiles/Path.png");
    //target.create(800, 600, sf::Color::Magenta);
//
    //for(int y = 0; y < TILE_HEIGHT; y++) {
    //    for(int x = 0; x < TILE_WIDTH; x++) {
    //        target.setPixel(x, y, grass.getPixel(x, y));
    //    }
    //}
//
    //for(int y = 0; y < TILE_HEIGHT; y++) {
    //    for(int x = 0; x < TILE_WIDTH * 16; x++) {
    //        target.setPixel(x, y + TILE_HEIGHT, path.getPixel(x, y));
    //    }
    //}
//
    //target.saveToFile("Data/Tiles.png");

    // Set up window and gui
    sf::RenderWindow window({1600, 900}, "SFML");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    ImGui::SFML::Init(window);

    // Set up screen system
    ScreenManager screens;
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
            screen->onEvent(e);
            keyboard.update(e);
            ImGui::SFML::ProcessEvent(e);
            switch (e.type) {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::KeyReleased:
                    if (e.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }

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
        screen->onInput(keyboard, window);
        screen->onUpdate(dt);
        ImGui::SFML::Update(window, dt);

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