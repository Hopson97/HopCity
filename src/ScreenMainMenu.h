#pragma once

#include "Screen.h"

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
};
