#include "TileMap.h"

#include "WorldGeneration.h"
#include <cmath>
#include <iostream>
#include <random>

#include "Common.h"
#include <SFML/Window/Mouse.hpp>

namespace {
    void addIsometricQuad(std::vector<sf::Vertex>* quads,
                          const sf::Vector2i& tilePosition)
    {
        auto pos = tileToScreenPosition(tilePosition);

        quads->emplace_back(pos);
        quads->emplace_back(sf::Vector2f{pos.x, pos.y + TILE_HEIGHT});
        quads->emplace_back(sf::Vector2f{pos.x + TILE_WIDTH, pos.y + TILE_HEIGHT});
        quads->emplace_back(sf::Vector2f{pos.x + TILE_WIDTH, pos.y});
    }

    void addGridLine(std::vector<sf::Vertex>* gridMap, sf::Vector2f startPosition,
                     sf::Vector2f endPosition)
    {
        sf::Color gridColour = {0, 0, 0, 100};

        startPosition.x += TILE_WIDTH / 2;
        endPosition.x += TILE_WIDTH / 2;
        gridMap->emplace_back(startPosition, gridColour);
        gridMap->emplace_back(endPosition, gridColour);
    }

    // https://gamedevelopment.tutsplus.com/tutorials/how-to-use-tile-bitmasking-to-auto-tile-your-level-layouts--cms-25673
    const sf::Vector2i TILE_OFFSETS[4] = {{0, 1}, {-1, 0}, {1, 0}, {0, -1}};

} // namespace

//  = = = = = = = = = = = = = = = = = = = = = = = = = = =
//
//                  TILE CHUNK MANAGER
//
//  = = = = = = = = = = = = = = = = = = = = = = = = = = =
void TileChunkManager::initWorld()
{
    m_tileTextures.loadFromFile("data/Textures/TileMap2.png");
    m_structureMap.loadFromFile("data/Textures/Structures.png");

    m_structureRect.setTexture(&m_structureMap);

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> seedDist(0, 4096);
    m_seed = seedDist(rng);

    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 2; x++) {
            addChunk({x, y});
        }
    }
}

void TileChunkManager::addChunk(const sf::Vector2i& chunkPos)
{
    if (m_chunks.find(chunkPos) == m_chunks.end()) {
        auto chunk = m_chunks.emplace(chunkPos, TileChunk{chunkPos, this});
        chunk.first->second.generateTerrain(m_seed);
    }
}

void TileChunkManager::regenerateTerrain()
{

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> seedDist(0, 4096);
    m_seed = seedDist(rng);

    m_structures.clear();
    sorted.clear();
    for (auto& chunk : m_chunks) {
        chunk.second.generateTerrain(m_seed);
    }
}

void TileChunkManager::setTile(const sf::Vector2i& tilePosition, TileType type)
{
    sf::Vector2i chunkPos = toChunkPosition(tilePosition);
    sf::Vector2i localPos = toLocalTilePosition(tilePosition);

    auto chunkItr = m_chunks.find(chunkPos);
    if (chunkItr == m_chunks.end()) {
        addChunk(chunkPos);
        chunkItr = m_chunks.find(chunkPos);
    }

    chunkItr->second.getTile(localPos)->type = type;
    chunkItr->second.updateTile(localPos);

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            sf::Vector2i chunkPos = toChunkPosition(tilePosition + sf::Vector2i{i, j});
            sf::Vector2i localPos =
                toLocalTilePosition(tilePosition + sf::Vector2i{i, j});

            auto chunkItr = m_chunks.find(chunkPos);
            if (chunkItr == m_chunks.end()) {
                addChunk(chunkPos);
                chunkItr = m_chunks.find(chunkPos);
            }
            chunkItr->second.updateTile(localPos);
        }
    }
}

Tile* TileChunkManager::getTile(const sf::Vector2i& tilePosition)
{
    static Tile noTile;

    sf::Vector2i chunkPos = toChunkPosition(tilePosition);
    sf::Vector2i localPos = toLocalTilePosition(tilePosition);

    auto chunkItr = m_chunks.find(chunkPos);
    return chunkItr != m_chunks.end() ? chunkItr->second.getTile(localPos) : &noTile;
}

bool TileChunkManager::canPlaceStructure(const sf::Vector2i& basePosition,
                                         StructureType type)
{
    const StructureDef* def = &StructureRegistry::instance().getStructure(type);

    auto correctTileType =
        def->placement == StructurePlacement::Land ? TileType::Land : TileType::Water;

    for (int y = 0; y < def->baseSize.y; y++) {
        for (int x = 0; x < def->baseSize.x; x++) {
            sf::Vector2i realPosition = basePosition - sf::Vector2i{x, y};

            // If it is the wrong tile type or there is a structure placed at the position
            if (getTile(realPosition)->type != correctTileType ||
                getStructurePlot(realPosition)) {
                return false;
            }
        }
    }
    return true;
}

bool& TileChunkManager::getStructurePlot(const sf::Vector2i& position)
{
    static bool noPlot = true;

    sf::Vector2i chunkPos = toChunkPosition(position);
    sf::Vector2i localPos = toLocalTilePosition(position);

    auto chunkItr = m_chunks.find(chunkPos);
    return chunkItr != m_chunks.end() ? chunkItr->second.getStructurePlot(localPos)
                                      : noPlot;
}

void TileChunkManager::draw(sf::RenderWindow* window)
{
    sf::RenderStates states = sf::RenderStates::Default;
    states.texture = &m_tileTextures;

    // Draw the chunks
    for (auto& chunk : m_chunks) {
        chunk.second.draw(*window, states);
        if (showDetail) {
            m_gridMap.setPosition(tileToScreenPosition(
                {(chunk.first.x - 2) * CHUNK_SIZE, chunk.first.y * CHUNK_SIZE}));
            m_gridMap.draw(*window);
        }
    }

    // Draw the structures
    for (const auto& structure : sorted) {
        const auto& str = m_structures[structure];
        const StructureDef* def = &StructureRegistry::instance().getStructure(str.type);

        m_structureRect.setSize(
            {TILE_WIDTH * def->textureSize.x, TILE_HEIGHT * def->textureSize.y});

        m_structureRect.setTextureRect(
            {(int)TILE_WIDTH * str.variant * (int)def->textureSize.x,
             (int)TILE_HEIGHT * def->textureIndex,
             (int)def->textureSize.x * (int)TILE_WIDTH,
             (int)def->textureSize.y * (int)TILE_HEIGHT});

        m_structureRect.setOrigin({0, m_structureRect.getSize().y - TILE_HEIGHT});
        m_structureRect.setPosition(tileToScreenPosition(structure));

        if (def->baseSize.x > 1) {
            m_structureRect.move(-def->textureSize.x * TILE_WIDTH / 4, 0);
        }
        window->draw(m_structureRect);
    }
}

void TileChunkManager::placeStructure(StructureType type, const sf::Vector2i& position)
{
    if (m_structures.find(position) == m_structures.end()) {
        Structure* structure =
            &m_structures.emplace(std::make_pair(position, Structure{type}))
                 .first->second;
        sorted.insert(position);
        const StructureDef* def = &StructureRegistry::instance().getStructure(type);

        for (int y = 0; y < def->baseSize.y; y++) {
            for (int x = 0; x < def->baseSize.x; x++) {
                sf::Vector2i realPosition = position - sf::Vector2i{x, y};
                getStructurePlot(realPosition) = true;
            }
        }

        // Change the texture of the tile
        if (def->variantType == VairantType::Random) {

            std::random_device rd;
            std::mt19937 rng{rd()};
            std::uniform_int_distribution<int> varietyDist(0, def->variations - 1);
            structure->variant = varietyDist(rng);
        }
        else if (def->variantType == VairantType::Neighbour) {
            structure->variant = 0;

            // Update the structure based on its neighbours
            for (int i = 0; i < 4; i++) {
                auto neighbour = m_structures.find(position + TILE_OFFSETS[i]);
                if (neighbour != m_structures.end() &&
                    neighbour->second.type == structure->type) {
                    structure->variant += (int)std::pow(2, i);
                }

                if (neighbour != m_structures.end() &&
                    StructureRegistry::instance()
                            .getStructure(neighbour->second.type)
                            .variantType == VairantType::Neighbour) {
                    neighbour->second.variant = 0;

                    for (int j = 0; j < 4; j++) {
                        auto subNeighbour = m_structures.find(position + TILE_OFFSETS[i] +
                                                              TILE_OFFSETS[j]);

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

//  = = = = = = = = = = = = = = = = = = = = = = = = = = =
//
//                  TILE CHUNK
//
//  = = = = = = = = = = = = = = = = = = = = = = = = = = =

TileChunk::TileChunk(const sf::Vector2i& position, TileChunkManager* chunkManager)
    : m_chunkPosition(position)
    , mp_chunkManager(chunkManager)
{
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            addIsometricQuad(&m_tileVerts, {x, y});
            updateTile({x, y});
        }
    }
    setPosition(
        tileToScreenPosition({(position.x - 2) * CHUNK_SIZE, position.y * CHUNK_SIZE}));
}

void TileChunk::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    states.transform *= getTransform();
    window.draw(m_tileVerts.data(), m_tileVerts.size(), sf::Quads, states);
}

void TileChunk::updateTile(const sf::Vector2i& position)
{
    // Set the tile'structure variant
    Tile* tile = getGlobalTile(position);
    const TileDef* def = &getTileDef(tile->type);

    if (def->variantType == VairantType::Random) {
        std::random_device rd;
        std::mt19937 rng{rd()};
        std::uniform_int_distribution<int> varietyDist(0, def->variations - 1);
        tile->variant = varietyDist(rng);
    }
    else if (def->variantType == VairantType::Neighbour) {
        tile->variant = 0;

        for (int i = 0; i < 4; i++) {
            Tile* neighbour = getGlobalTile(position + TILE_OFFSETS[i]);
            if (neighbour && tile->type == neighbour->type) {
                tile->variant += (int)std::pow(2, i);
            }

            if (neighbour &&
                getTileDef(neighbour->type).variantType == VairantType::Neighbour) {
                neighbour->variant = 0;
                for (int j = 0; j < 4; j++) {
                    Tile* subNeighbour =
                        getGlobalTile(position + TILE_OFFSETS[i] + TILE_OFFSETS[j]);
                    if (subNeighbour && subNeighbour->type == neighbour->type) {
                        neighbour->variant += (int)std::pow(2, j);
                    }
                }
            }
        }
    }

    // Update the tile texture coords
    int vertexIndex = (position.y * CHUNK_SIZE + position.x) * 4;
    if (vertexIndex >= (int)m_tileVerts.size()) {
        return;
    }
    sf::Vertex* vertex = &m_tileVerts[vertexIndex];
    float idx = static_cast<float>(def->textureIndex);
    vertex[0].texCoords = {tile->variant * TILE_WIDTH, TILE_HEIGHT * idx};
    vertex[1].texCoords = {tile->variant * TILE_WIDTH, TILE_HEIGHT * (idx + 1)};
    vertex[2].texCoords = {tile->variant * TILE_WIDTH + TILE_WIDTH,
                           TILE_HEIGHT * (idx + 1)};
    vertex[3].texCoords = {tile->variant * TILE_WIDTH + TILE_WIDTH, TILE_HEIGHT * idx};
}

bool& TileChunk::getStructurePlot(const sf::Vector2i& position)
{
    // Check for out of bounds
    static bool oobPlot = false;
    if (position.y < 0 || position.y >= CHUNK_SIZE || position.x < 0 ||
        position.x >= CHUNK_SIZE) {
        return oobPlot;
    }

    // Get the tile
    return m_structurePlots.at(position.y * CHUNK_SIZE + position.x);
}

Tile* TileChunk::getTile(const sf::Vector2i& position)
{
    // Check for out of bounds
    static Tile e;
    if (position.y < 0 || position.y >= CHUNK_SIZE || position.x < 0 ||
        position.x >= CHUNK_SIZE) {
        return &e;
    }

    // Get the tile
    return &m_tiles.at(position.y * CHUNK_SIZE + position.x);
}

Tile* TileChunk::getGlobalTile(const sf::Vector2i& position)
{
    return mp_chunkManager->getTile(toGlobalTilePosition(m_chunkPosition, position));
}

//   ;

void TileChunk::generateTerrain(int seed)
{
    m_tiles = generateWorld(m_chunkPosition, mp_chunkManager, seed);

    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            updateTile({x, y});
        }
    }
}

GridMap::GridMap()
{
    for (int i = 0; i < CHUNK_SIZE + 1; i++) {
        addGridLine(&m_grid, tileToScreenPosition({0, i}),
                    tileToScreenPosition({CHUNK_SIZE, i}));
        addGridLine(&m_grid, tileToScreenPosition({i, 0}),
                    tileToScreenPosition({i, CHUNK_SIZE}));
    }
}

void GridMap::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    states.transform *= getTransform();
    window.draw(m_grid.data(), m_grid.size(), sf::Lines, states);
}
