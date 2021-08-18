#include "ScreenMainMenu.h"
#include "ScreenGame.h"
#include <imgui_sfml/imgui.h>



ScreenMainMenu::ScreenMainMenu(ScreenManager* stack, const sf::RenderWindow& window)
    : Screen(stack)
    , m_buttonAnimation(32, 64, false)
{
    for (int i = 0; i < 6; i++) {
        m_buttonAnimation.addFrame(0, i, sf::milliseconds(100));
    }

    m_newGameTexture.loadFromFile("data/Textures/NewGame.png");
    m_loadGameTexture.loadFromFile("data/Textures/LoadGame.png");

    m_newGameButton.setTexture(&m_newGameTexture);
    m_newGameButton.setSize({256, 512});
    m_newGameButton.setPosition(300, window.getSize().y / 2 - 256);

    m_loadGameButton.setTexture(&m_loadGameTexture);
    m_loadGameButton.setSize({256, 512});
    m_loadGameButton.setPosition(600, window.getSize().y / 2 - 256);

    m_wall.setSize({400, window.getSize().y});

}

void ScreenMainMenu::onGUI() {}

void ScreenMainMenu::onEvent(const sf::Event& e)
{
    if (e.type == sf::Event::MouseButtonReleased &&
        e.mouseButton.button == sf::Mouse::Left) {
        float x = (float)e.mouseButton.x;
        float y = (float)e.mouseButton.y;
        if (m_newGameButton.getGlobalBounds().contains(x, y)) {
            m_pScreens->pushScreen(std::make_unique<ScreenGame>(m_pScreens));
        }
        else if (m_loadGameButton.getGlobalBounds().contains(x, y)) {
            //m_pScreens->pushScreen(std::make_unique<ScreenGame>(m_pScreens));
        }
    }
}

void ScreenMainMenu::onInput([[maybe_unused]] const Keyboard& keyboard,
                             const sf::RenderWindow& window)
{

    auto mp = sf::Mouse::getPosition(window);

    if (m_newGameButton.getGlobalBounds().contains((float)mp.x, (float)mp.y)) {
        m_newGameButton.setTextureRect(m_buttonAnimation.progressFrame());
    }
    else {
        m_buttonAnimation.reset();
        m_newGameButton.setTextureRect(m_buttonAnimation.progressFrame());
    }
}

void ScreenMainMenu::onRender(sf::RenderWindow* window)
{
    for (int i = 0; i < 1600 / 400; i++) {
        m_wall.setPosition(i * 400, 0);
        
        m_wall.setFillColor(i % 2 == 0 ? sf::Color{100, 100, 100} : sf::Color::White);
        window->draw(m_wall);
    }
    window->draw(m_newGameButton);
    window->draw(m_loadGameButton);

}
