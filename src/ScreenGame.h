#pragma once

#include "Animation.h"
#include "Screen.h"

#include "Game/World.h"

class ScreenGame final : public Screen {
    enum class Menu {
        MainMenu,
        SettingsMenu,
    };

  public:
    ScreenGame(ScreenManager* screens);

    void onGUI() override;
    void onEvent(const sf::Event& e) override;
    void onUpdate(const sf::Time& dt) override;
    void onRender(sf::RenderWindow* window) override;

  private:
    sf::View m_view;
    Menu m_activeMenu = Menu::MainMenu;

    sf::RectangleShape m_sprite;

    sf::Texture m_tileTexture;
};
