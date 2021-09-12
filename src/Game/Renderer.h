#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <vector>

struct Structure;

struct RenderCommand {
    enum class Type {
        Structure,
    };

    sf::Vector2i position;
    Type type;

    // Reference to the enum type being drawn
    uint16_t id;
    uint8_t variant = 0;
};

class Renderer {
  public:
    Renderer();

    void addStructure(const sf::Vector2i& position, const Structure& structure);

    void render(sf::RenderWindow& window);

  private:
    void addCommand(const RenderCommand& command);
    void renderStructure(const RenderCommand& command, sf::RenderWindow& window);

    std::vector<RenderCommand> m_renderCommands;

    sf::RectangleShape m_structureRect;
    sf::Texture m_structureMap;
};