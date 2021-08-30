#include "StructureMap.h"

#include "TileMap.h"
#include <random>

StructureMap::StructureMap()
{
    m_structureMap.loadFromFile("data/Textures/Structures.png");
    m_structureRect.setTexture(&m_structureMap);
}

void StructureMap::clear()
{
    m_structures.clear();
    m_sortedStructList.clear();
}

void StructureMap::draw(sf::RenderWindow* target)
{

    // Draw the structures
    for (const auto& position : m_sortedStructList)
    {
        const Structure& str = m_structures.at(position);
        const StructureDef& def = StructureRegistry::instance().getStructure(str.type);

        m_structureRect.setSize({TILE_WIDTH * def.textureSize.x, TILE_HEIGHT * def.textureSize.y});

        m_structureRect.setTextureRect(
            {(int)TILE_WIDTH * str.variant * (int)def.textureSize.x, (int)TILE_HEIGHT * def.textureIndex,
             (int)def.textureSize.x * (int)TILE_WIDTH, (int)def.textureSize.y * (int)TILE_HEIGHT});

        m_structureRect.setOrigin({0, m_structureRect.getSize().y - TILE_HEIGHT});
        m_structureRect.setPosition(tileToScreenPosition(position));

        if (m_currentlySelectedTile == position)
        {
            m_structureRect.setFillColor(sf::Color::Green);
        }
        else
        {
            m_structureRect.setFillColor(sf::Color::White);
        }

        if (def.baseSize.x > 1)
        {
            m_structureRect.move(-def.textureSize.x * TILE_WIDTH / 4, 0);
        }
        target->draw(m_structureRect);
    }
}

StructureType StructureMap::removeStructure(const sf::Vector2i& tilePosition, TileMap& manager)
{
    auto itr = m_structures.find(tilePosition);

    if (itr != m_structures.end())
    {
        auto type = itr->second.type;
        m_structures.erase(itr);

        for (auto itr = m_sortedStructList.begin(); itr != m_sortedStructList.end();)
        {
            if (*itr == tilePosition)
            {
                m_sortedStructList.erase(itr);
                break;
            }
            itr++;
        }

        manager.getStructurePlot(tilePosition) = false;

        return type;
    }

    return StructureType::None;
}

void StructureMap::setCurrentlySelectedTile(const sf::Vector2i& position)
{
    m_currentlySelectedTile = position;
}

const Structure& StructureMap::getStructure(const sf::Vector2i& position)
{
    static Structure s{StructureType::None};
    auto itr = m_structures.find(position);
    return itr != m_structures.end() ? itr->second : s;
}

void StructureMap::placeStructure(StructureType type, const sf::Vector2i& position, TileMap& manager)
{
    if (m_structures.find(position) == m_structures.end())
    {
        Structure* structure = &m_structures.emplace(std::make_pair(position, Structure{type})).first->second;

        bool inserted = false;
        for (auto itr = m_sortedStructList.begin(); itr != m_sortedStructList.end();)
        {
            if (itr->x + itr->y >= position.x + position.y)
            {
                m_sortedStructList.insert(itr, position);
                inserted = true;
                break;
            }
            itr++;
        }
        if (!inserted)
        {
            m_sortedStructList.push_back(position);
        }

        const StructureDef& def = StructureRegistry::instance().getStructure(type);

        for (int y = 0; y < def.baseSize.y; y++)
        {
            for (int x = 0; x < def.baseSize.x; x++)
            {
                sf::Vector2i realPosition = position - sf::Vector2i{x, y};
                manager.getStructurePlot(realPosition) = true;
            }
        }

        // Change the texture of the tile
        if (def.variantType == VairantType::Random)
        {

            std::random_device rd;
            std::mt19937 rng{rd()};
            std::uniform_int_distribution<int> varietyDist(0, def.variations - 1);
            structure->variant = varietyDist(rng);
        }
        else if (def.variantType == VairantType::Neighbour)
        {
            structure->variant = 0;

            // Update the structure based on its neighbours
            for (int i = 0; i < 4; i++)
            {
                auto neighbour = m_structures.find(position + TILE_OFFSETS[i]);
                if (neighbour != m_structures.end() && neighbour->second.type == structure->type)
                {
                    structure->variant += (int)std::pow(2, i);
                }

                if (neighbour != m_structures.end() &&
                    StructureRegistry::instance().getStructure(neighbour->second.type).variantType ==
                        VairantType::Neighbour)
                {
                    neighbour->second.variant = 0;

                    for (int j = 0; j < 4; j++)
                    {
                        auto subNeighbour = m_structures.find(position + TILE_OFFSETS[i] + TILE_OFFSETS[j]);

                        if (subNeighbour != m_structures.end() &&
                            subNeighbour->second.type == neighbour->second.type)
                        {
                            neighbour->second.variant += (int)std::pow(2, j);
                        }
                    }
                }
            }
        }
    }
}
