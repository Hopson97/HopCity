#include "Renderer.h"
#include "Registry/StructureRegistry.h"

Renderer::Renderer()
{
    m_structureMap.loadFromFile("data/Textures/Structures.png");
    m_structureRect.setTexture(&m_structureMap);
}

void Renderer::addStructure(const sf::Vector2i& position, const Structure& structure)
{
    RenderCommand command;
    command.type = RenderCommand::Type::Structure;
    command.position = position;
    command.id = (uint8_t)structure.type;
    command.variant = structure.variant;

    addCommand(command);
}

void Renderer::render(sf::RenderWindow& window)
{
    for (auto& command : m_renderCommands) {
        switch (command.type) {
            case RenderCommand::Type::Structure:
                renderStructure(command, window);
                break;

            default:
                break;
        }
    }
    m_renderCommands.clear();
}

void Renderer::addCommand(const RenderCommand& command)
{
    auto& p = command.position;
    bool inserted = false;
    for (auto itr = m_renderCommands.begin(); itr != m_renderCommands.end();) {
        if (itr->position.x + itr->position.y >= p.x + p.y) {
            m_renderCommands.insert(itr, command);
            inserted = true;
            break;
        }
        itr++;
    }
    if (!inserted) {
        m_renderCommands.push_back(command);
    }
}

void Renderer::renderStructure(const RenderCommand& str, sf::RenderWindow& window)
{
    const StructureDef& def = StructureRegistry::instance().getStructure((StructureType)str.id);

    m_structureRect.setSize({TILE_WIDTH * def.textureSize.x, TILE_HEIGHT * def.textureSize.y});

    m_structureRect.setTextureRect(
        {(int)TILE_WIDTH * str.variant * (int)def.textureSize.x, (int)TILE_HEIGHT * def.textureIndex,
         (int)def.textureSize.x * (int)TILE_WIDTH, (int)def.textureSize.y * (int)TILE_HEIGHT});

    m_structureRect.setOrigin({0, m_structureRect.getSize().y - TILE_HEIGHT});
    m_structureRect.setPosition(tileToScreenPosition(str.position));

    // if (m_currentlySelectedTile == position) {
    //    m_structureRect.setFillColor(sf::Color::Green);
    //}
    // else {
    //    m_structureRect.setFillColor(sf::Color::White);
    //}

    if (def.baseSize.x > 1) {
        m_structureRect.move(-def.textureSize.x * TILE_WIDTH / 4, 0);
    }
    window.draw(m_structureRect);
}
