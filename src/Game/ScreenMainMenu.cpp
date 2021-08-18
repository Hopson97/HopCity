#include "ScreenMainMenu.h"
#include "ScreenGame.h"
#include <imgui_sfml/imgui.h>

ScreenMainMenu::ScreenMainMenu(ScreenManager* stack, const sf::RenderWindow& window)
    : Screen(stack)
    , newgameanim(32, 64, false)
    , loopedani(32, 64, false)
{
    for (int i = 0; i < 6; i++) {
        newgameanim.addFrame(0, i, sf::milliseconds(100));
    }

    testTexutre.loadFromFile("data/Textures/NewGame.png");
    testFrame.setTexture(&testTexutre);
    testFrame.setSize({256, 512});
    m_wall.setSize({400, window.getSize().y});
    testFrame.setPosition(300, window.getSize().y / 2 - 256);
}

void ScreenMainMenu::onGUI() {}

void ScreenMainMenu::onEvent(const sf::Event& e)
{
    if (e.type == sf::Event::MouseButtonReleased &&
        e.mouseButton.button == sf::Mouse::Left) {
        float x = (float)e.mouseButton.x;
        float y = (float)e.mouseButton.y;
        if (testFrame.getGlobalBounds().contains(x, y)) {
            m_pScreens->pushScreen(std::make_unique<ScreenGame>(m_pScreens));
        }
    }
}

void ScreenMainMenu::onInput([[maybe_unused]] const Keyboard& keyboard,
                             const sf::RenderWindow& window)
{

    auto mp = sf::Mouse::getPosition(window);

    if (testFrame.getGlobalBounds().contains((float)mp.x, (float)mp.y)) {
        testFrame.setTextureRect(newgameanim.progressFrame());
    }
    else {
        newgameanim.reset();
        testFrame.setTextureRect(newgameanim.progressFrame());
    }
}

void ScreenMainMenu::onRender(sf::RenderWindow* window)
{
    for (int i = 0; i < 1600 / 400; i++) {
        m_wall.setPosition(i * 400, 0);
        
        m_wall.setFillColor(i % 2 == 0 ? sf::Color{100, 100, 100} : sf::Color::White);
        window->draw(m_wall);
    }
    window->draw(testFrame);

}
