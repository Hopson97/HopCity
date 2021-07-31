#pragma once

#include "Animation.h"
#include "Screen.h"

class ScreenGame final : public Screen {
    enum class Menu {
        MainMenu,
        SettingsMenu,
    };

  public:
    ScreenGame(ScreenManager* screens);

    void onGUI() override;
    void onUpdate(const sf::Time& dt) override;
    void onRender(sf::RenderWindow* window) override;

  private:
    Menu m_activeMenu = Menu::MainMenu;

    sf::RectangleShape m_sprite;
    sf::Clock m_clock;
    sf::Texture m_recruitTexture;

    Animation m_recruitWalkAnimation;
};
