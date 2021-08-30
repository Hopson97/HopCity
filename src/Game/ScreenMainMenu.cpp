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

    m_newGameButton.init("data/Textures/NewGame.png", 1);
    m_loadGameButton.init("data/Textures/LoadGame.png", 2);
    m_exitGameButton.init("data/Textures/LoadGame.png", 3);
    m_wall.setSize({400.0f, (float)window.getSize().y});
}

void ScreenMainMenu::onGUI() {}

void ScreenMainMenu::onEvent(const sf::Event& e)
{
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        float x = (float)e.mouseButton.x;
        float y = (float)e.mouseButton.y;

        if (m_newGameButton.contains(x, y)) {
            m_pScreens->pushScreen(std::make_unique<ScreenGame>(m_pScreens));
        }
        else if (m_loadGameButton.contains(x, y)) {
        }
        else if (m_exitGameButton.contains(x, y)) {
            m_pScreens->popScreen();
        }
    }
}

void ScreenMainMenu::onInput([[maybe_unused]] const Keyboard& keyboard, const sf::RenderWindow& window)
{

    auto mp = sf::Mouse::getPosition(window);

    if (m_newGameButton.contains(mp.x, mp.y)) {
        m_newGameButton.sprite.setTextureRect(m_buttonAnimation.progressFrame());
    }
    else {
        m_buttonAnimation.reset();
        m_newGameButton.sprite.setTextureRect(m_buttonAnimation.progressFrame());
    }
}

void ScreenMainMenu::onRender(sf::RenderWindow* window)
{
    for (int i = 0; i < 1600 / 400; i++) {
        m_wall.setPosition(i * 400, 0);

        m_wall.setFillColor(i % 2 == 0 ? sf::Color{100, 100, 100} : sf::Color::White);
        window->draw(m_wall);
    }
    window->draw(m_newGameButton.sprite);
    window->draw(m_loadGameButton.sprite);
    window->draw(m_exitGameButton.sprite);
}

void ScreenMainMenu::MainMenuButton::init(const std::string& texturef, int index)
{
    texture.loadFromFile(texturef);

    sprite.setTexture(&texture);
    sprite.setSize({256.0f, 512.0f});
    sprite.setPosition(300.0f * index, 900 / 2.0f - 256.0f);
}

bool ScreenMainMenu::MainMenuButton::contains(int mx, int my)
{
    return sprite.getGlobalBounds().contains(mx, my);
}
