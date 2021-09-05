#pragma once

#include "../Animation.h"
#include "../Screen.h"

class ScreenMainMenu final : public Screen {
    struct MainMenuButton {
        sf::RectangleShape sprite;
        sf::Texture texture;

        void init(const std::string& texture, int index);
        bool contains(int mx, int my);
    };

    enum class Menu {
        MainMenu,
        SettingsMenu,
    };

  public:
    ScreenMainMenu(ScreenManager* screens, const sf::RenderWindow& window);

    void onGUI() override;
    void onEvent(const sf::Event& e) override;
    void* onInput(const Keyboard& keyboard, const sf::RenderWindow& window) override;
    void onRender(sf::RenderWindow* window) override;

  private:
    MainMenuButton m_newGameButton;
    MainMenuButton m_loadGameButton;
    MainMenuButton m_exitGameButton;
    Animation m_buttonAnimation;
    sf::RectangleShape m_wall;
};
