#pragma once

#include "Keyboard.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <stack>
#include <vector>

class GUI;

class Screen;

class ScreenManager final {
    struct Action {
        enum class Kind {
            Push,
            Pop,
            Change,
        };
        Kind kind;
        std::unique_ptr<Screen> screen;
    };

  public:
    void pushScreen(std::unique_ptr<Screen> screen);
    void popScreen();
    void changeScreen(std::unique_ptr<Screen> screen);

    void update();

    Screen& peekScreen();

    bool isEmpty() const;

  private:
    std::stack<std::unique_ptr<Screen>> m_screens;
    std::vector<Action> m_actions;
};

class Screen {
  public:
    Screen(ScreenManager* screens);
    virtual ~Screen() = default;

    virtual void onGUI() {}
    virtual void onEvent([[maybe_unused]] const sf::Event& e) {}
    virtual void onInput([[maybe_unused]] const Keyboard& keyboard,
                         [[maybe_unused]] const sf::RenderWindow& window)
    {
    }
    virtual void onUpdate([[maybe_unused]] const sf::Time& dt) {}
    virtual void onFixedUpdate([[maybe_unused]] const sf::Time& dt) {}
    virtual void onRender(sf::RenderWindow* window) = 0;

  protected:
    ScreenManager* m_pScreens;
};

bool imguiBeginMenu(const char* name);
bool imguiButtonCustom(const char* text);