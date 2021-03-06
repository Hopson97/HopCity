#include "Camera.h"

#include "../Keyboard.h"

#include "WorldGeneration.h"
#include <imgui_sfml/imgui.h>

#include "Common.h"

Camera::Camera()
{
    m_view.setCenter(tileToScreenPosition({0, 0}));
    m_view.setSize({1600, 900});
    m_view.zoom(zoomLevel);
}

void Camera::setViewToCamera(sf::RenderWindow& window) { window.setView(m_view); }

void Camera::onEvent(const sf::Event& e)
{
    if (e.type == sf::Event::MouseWheelScrolled && !ImGui::GetIO().WantCaptureMouse) {
        if (e.mouseWheelScroll.delta > 0 && zoomLevel == 8) {
            m_view.zoom(1.0f / 8.0f);
            zoomLevel *= 1.0f / 8.0f;
        }
        else if (e.mouseWheelScroll.delta > 0 && zoomLevel > 0.065) {
            m_view.zoom(0.5f);
            zoomLevel *= 0.5f;
        }
        else if (e.mouseWheelScroll.delta < 0 && zoomLevel < 1) {
            m_view.zoom(2.0f);
            zoomLevel *= 2.0f;
        }
        else if (e.mouseWheelScroll.delta < 0 && (int)zoomLevel == 1) {
            m_view.zoom(8.0f);
            zoomLevel = 8.0f;
        }
    }
}

void Camera::onInput(const Keyboard& keyboard, const sf::RenderWindow& window)
{
    if (!ImGui::GetIO().WantCaptureMouse) {

        // Move the view if it is on the edge of the screen
        constexpr int GAP = 25;
        auto mousePosition = sf::Mouse::getPosition(window);
        if (mousePosition.x < GAP) {
            m_view.move(-5, 0);
        }
        else if (mousePosition.x > (int)window.getSize().x - GAP) {
            m_view.move(5, 0);
        }
        if (mousePosition.y < GAP) {
            m_view.move(0, -5);
        }
        else if (mousePosition.y > (int)window.getSize().y - GAP) {
            m_view.move(0, 5);
        }
    }

    if (keyboard.isKeyDown(sf::Keyboard::A)) {
        m_view.move(-4, 0);
    }
    else if (keyboard.isKeyDown(sf::Keyboard::D)) {
        m_view.move(4, 0);
    }

    if (keyboard.isKeyDown(sf::Keyboard::S)) {
        m_view.move(0, 4);
    }
    else if (keyboard.isKeyDown(sf::Keyboard::W)) {
        m_view.move(0, -4);
    }
}