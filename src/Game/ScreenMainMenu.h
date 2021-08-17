#pragma once

#include "../Screen.h"
#include "../Animation.h"
class ScreenMainMenu final : public Screen {
    enum class Menu {
        MainMenu,
        SettingsMenu,
    };

  public:
    ScreenMainMenu(ScreenManager* screens);

    void onGUI() override;
    void onRender(sf::RenderWindow* window) override;

  private:
    Animation newgameanim;
    Animation loopedani;
    sf::Texture testTexutre;
    sf::RectangleShape testFrame;
};
