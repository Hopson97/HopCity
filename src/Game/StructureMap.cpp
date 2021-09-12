#include "StructureMap.h"

#include "Renderer.h"
#include "TileMap.h"
#include <random>

void StructureMap::clear() { m_structures.clear(); }

void StructureMap::draw(Renderer& renderer)
{
    for (auto& [position, structure] : m_structures) {
        renderer.addStructure(position, structure);
    }
}

int StructureMap::structureTeamAt(const sf::Vector2i& tilePosition)
{
    auto itr = m_structures.find(tilePosition);
    if (itr != m_structures.end()) {
        return itr->second.team;
    }
    return -1;
}

StructureType StructureMap::removeStructure(const sf::Vector2i& tilePosition, TileMap& manager)
{
    auto itr = m_structures.find(tilePosition);

    if (itr != m_structures.end()) {
        auto type = itr->second.type;
        m_structures.erase(itr);
        manager.getStructurePlot(tilePosition) = false;
        return type;
    }
    return StructureType::None;
}

const Structure& StructureMap::getStructure(const sf::Vector2i& position)
{
    static Structure s{StructureType::None};
    auto itr = m_structures.find(position);
    return itr != m_structures.end() ? itr->second : s;
}

void StructureMap::placeStructure(StructureType type, const sf::Vector2i& position, TileMap& manager,
                                  int team)
{
    if (m_structures.find(position) == m_structures.end()) {
        Structure* structure = &m_structures.emplace(std::make_pair(position, Structure{type})).first->second;
        structure->team = team;

        const StructureDef& def = StructureRegistry::instance().getStructure(type);

        for (int y = 0; y < def.baseSize.y; y++) {
            for (int x = 0; x < def.baseSize.x; x++) {
                sf::Vector2i realPosition = position - sf::Vector2i{x, y};
                manager.getStructurePlot(realPosition) = true;
            }
        }

        // Change the texture of the tile
        if (def.variantType == VairantType::Random) {

            std::random_device rd;
            std::mt19937 rng{rd()};
            std::uniform_int_distribution<int> varietyDist(0, def.variations - 1);
            structure->variant = varietyDist(rng);
        }
        else if (def.variantType == VairantType::Neighbour) {
            structure->variant = 0;

            // Update the structure based on its neighbours
            for (int i = 0; i < 4; i++) {
                auto neighbour = m_structures.find(position + TILE_OFFSETS[i]);
                if (neighbour != m_structures.end() && neighbour->second.type == structure->type) {
                    structure->variant += (int)std::pow(2, i);
                }

                if (neighbour != m_structures.end() &&
                    StructureRegistry::instance().getStructure(neighbour->second.type).variantType ==
                        VairantType::Neighbour) {
                    neighbour->second.variant = 0;

                    for (int j = 0; j < 4; j++) {
                        auto subNeighbour = m_structures.find(position + TILE_OFFSETS[i] + TILE_OFFSETS[j]);

                        if (subNeighbour != m_structures.end() &&
                            subNeighbour->second.type == neighbour->second.type) {
                            neighbour->second.variant += (int)std::pow(2, j);
                        }
                    }
                }
            }
        }
    }
}
