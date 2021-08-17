#include "Game/ScreenGame.h"
#include "Game/ScreenMainMenu.h"
#include "Game/World.h"
#include "Keyboard.h"
#include "Profiler.h"
#include "Screen.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui_sfml/imgui-SFML.h>
#include <imgui_sfml/imgui.h>
#include <nuklear_sfml/nuklear_def.h>
#include <nuklear_sfml/nuklear_sfml_gl2.h>

int main()
{
    // Set up window and gui
    sf::RenderWindow window({1600, 900}, "game");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    ImGui::SFML::Init(window);


    nk_context* nuklear = nk_sfml_init(&window);
    {
        struct nk_font_atlas* atlas;
        nk_sfml_font_stash_begin(&atlas);
        nk_sfml_font_stash_end();
    }

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
    screens.pushScreen(std::make_unique<ScreenGame>(&screens));
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
        nk_input_begin(nuklear);
        sf::Event e;
        while (window.pollEvent(e)) {

            screen->onEvent(e);
            keyboard.update(e);
            ImGui::SFML::ProcessEvent(e);
            nk_sfml_handle_event(&e);
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

        // Fixed time update
        while (lag >= timePerUpdate) {
            lag -= timePerUpdate;
            screen->onFixedUpdate(elapsed);
        }

        // Rendering
        window.clear({64, 164, 223});
        screen->onRender(&window);

        nk_overview(nuklear);

        if (nk_begin(nuklear, "Graphics Options", nk_rect(10, 150, 300, 200), 0)) {
            nk_layout_row_dynamic(nuklear, 14, 1);
            nk_labelf(nuklear, NK_STATIC, "Press 'L' to unlock mouse.");
            nk_labelf(nuklear, NK_STATIC, "Press 'ESC' to exit.");

            if (nk_button_label(nuklear, "Ok boomer")) {
                std::printf("ok");
            }
        }
        nk_end(nuklear);

        screen->onGUI();
        ImGui::SFML::Render(window);
        nk_sfml_render(NK_ANTI_ALIASING_ON);

        window.display();
        screens.update();
    }

    nk_sfml_shutdown();
}