#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Event.hpp>

class Keyboard;

class Camera {
  public:
    Camera();

    void setViewToCamera(sf::RenderWindow& window);
    void onEvent(const sf::Event& e);
    void onInput(const Keyboard& keyboard, const sf::RenderWindow& window);

    float zoomLevel = 0.5;

  private:
    sf::View m_view;
};
