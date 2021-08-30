#include "TileMap.h"

#include "WorldGeneration.h"
#include <cmath>
#include <iostream>
#include <random>

#include "Common.h"
#include <SFML/Window/Mouse.hpp>

namespace {
    void addIsometricQuad(std::vector<sf::Vertex>* quads, const sf::Vector2i& tilePosition)
    {
        auto pos = tileToScreenPosition(tilePosition);

        quads->emplace_back(pos);
        quads->emplace_back(sf::Vector2f{pos.x, pos.y + TILE_HEIGHT});
        quads->emplace_back(sf::Vector2f{pos.x + TILE_WIDTH, pos.y + TILE_HEIGHT});
        quads->emplace_back(sf::Vector2f{pos.x + TILE_WIDTH, pos.y});
    }

    void addGridLine(std::vector<sf::Vertex>* gridMap, sf::Vector2f startPosition, sf::Vector2f endPosition)
    {
        sf::Color gridColour = {0, 0, 0, 100};

        startPosition.x += TILE_WIDTH / 2;
        endPosition.x += TILE_WIDTH / 2;
        gridMap->emplace_back(startPosition, gridColour);
        gridMap->emplace_back(endPosition, gridColour);
    }

    // https://gamedevelopment.tutsplus.com/tutorials/how-to-use-tile-bitmasking-to-auto-tile-your-level-layouts--cms-25673

} // namespace

//  = = = = = = = = = = = = = = = = = = = = = = = = = = =
//
//                  TILE CHUNK MANAGER
//
//  = = = = = = = = = = = = = = = = = = = = = = = = = = =
void TileMap::initWorld() { m_tileTextures.loadFromFile("data/Textures/TileMap2.png"); }

TileChunk& TileMap::addChunk(const sf::Vector2i& chunkPos)
{
    auto itr = m_chunks.find(chunkPos);
    if (itr == m_chunks.end()) {
        auto chunk = m_chunks.emplace(chunkPos, TileChunk{chunkPos, this});
        return chunk.first->second;
    }
    return itr->second;
}

/*
void TileMap::regenerateTerrain()
{

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> seedDist(0, 4096);
    m_seed = seedDist(rng);

    for (auto& chunk : m_chunks) {
        chunk.second.clearPlots();
        chunk.second.generateTerrain(m_seed);
    }
}
*/
void TileMap::setTile(const sf::Vector2i& tilePosition, TileType type)
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
            sf::Vector2i localPos = toLocalTilePosition(tilePosition + sf::Vector2i{i, j});

            auto chunkItr = m_chunks.find(chunkPos);
            if (chunkItr == m_chunks.end()) {
                addChunk(chunkPos);
                chunkItr = m_chunks.find(chunkPos);
            }
            chunkItr->second.updateTile(localPos);
        }
    }
}

Tile* TileMap::getTile(const sf::Vector2i& tilePosition)
{
    static Tile noTile;

    sf::Vector2i chunkPos = toChunkPosition(tilePosition);
    sf::Vector2i localPos = toLocalTilePosition(tilePosition);

    auto chunkItr = m_chunks.find(chunkPos);
    return chunkItr != m_chunks.end() ? chunkItr->second.getTile(localPos) : &noTile;
}

bool TileMap::canPlaceStructure(const sf::Vector2i& basePosition, StructureType type)
{
    const StructureDef& def = StructureRegistry::instance().getStructure(type);

    auto correctTileType = def.placement == StructurePlacement::Land ? TileType::Land : TileType::Water;

    for (int y = 0; y < def.baseSize.y; y++) {
        for (int x = 0; x < def.baseSize.x; x++) {
            sf::Vector2i realPosition = basePosition - sf::Vector2i{x, y};

            // If it is the wrong tile type or there is a structure placed at the position
            if (getTile(realPosition)->type != correctTileType || getStructurePlot(realPosition)) {
                return false;
            }
        }
    }
    return true;
}

bool& TileMap::getStructurePlot(const sf::Vector2i& position)
{
    static bool noPlot = true;

    sf::Vector2i chunkPos = toChunkPosition(position);
    sf::Vector2i localPos = toLocalTilePosition(position);

    auto chunkItr = m_chunks.find(chunkPos);
    return chunkItr != m_chunks.end() ? chunkItr->second.getStructurePlot(localPos) : noPlot;
}

void TileMap::draw(sf::RenderWindow* window)
{
    sf::RenderStates states = sf::RenderStates::Default;
    states.texture = &m_tileTextures;

    // Draw the chunks
    for (auto& chunk : m_chunks) {
        chunk.second.draw(*window, states);
        if (showDetail) {
            m_gridMap.setPosition(
                tileToScreenPosition({(chunk.first.x - 2) * CHUNK_SIZE, chunk.first.y * CHUNK_SIZE}));
            m_gridMap.draw(*window);
        }
    }
}

//  = = = = = = = = = = = = = = = = = = = = = = = = = = =
//
//                  TILE CHUNK
//
//  = = = = = = = = = = = = = = = = = = = = = = = = = = =

TileChunk::TileChunk(const sf::Vector2i& position, TileMap* chunkManager)
    : chunkPosition(position)
    , p_chunkManager(chunkManager)
{
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            addIsometricQuad(&m_tileVerts, {x, y});
            updateTile({x, y});
        }
    }
    setPosition(tileToScreenPosition({(position.x - 2) * CHUNK_SIZE, position.y * CHUNK_SIZE}));
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
    const TileDef& def = TileRegistry::instance().getTileDef(tile->type);

    if (def.variantType == VairantType::Random) {
        std::random_device rd;
        std::mt19937 rng{rd()};
        std::uniform_int_distribution<int> varietyDist(0, def.variations - 1);
        tile->variant = varietyDist(rng);
    }
    else if (def.variantType == VairantType::Neighbour) {
        tile->variant = 0;

        for (int i = 0; i < 4; i++) {
            Tile* neighbour = getGlobalTile(position + TILE_OFFSETS[i]);
            if (neighbour && tile->type == neighbour->type) {
                tile->variant += (int)std::pow(2, i);
            }

            const TileDef& neighbourDef = TileRegistry::instance().getTileDef(neighbour->type);
            if (neighbour && neighbourDef.variantType == VairantType::Neighbour) {
                neighbour->variant = 0;
                for (int j = 0; j < 4; j++) {
                    Tile* subNeighbour = getGlobalTile(position + TILE_OFFSETS[i] + TILE_OFFSETS[j]);
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
    float idx = static_cast<float>(def.textureIndex);
    vertex[0].texCoords = {tile->variant * TILE_WIDTH, TILE_HEIGHT * idx};
    vertex[1].texCoords = {tile->variant * TILE_WIDTH, TILE_HEIGHT * (idx + 1)};
    vertex[2].texCoords = {tile->variant * TILE_WIDTH + TILE_WIDTH, TILE_HEIGHT * (idx + 1)};
    vertex[3].texCoords = {tile->variant * TILE_WIDTH + TILE_WIDTH, TILE_HEIGHT * idx};
}

bool& TileChunk::getStructurePlot(const sf::Vector2i& position)
{
    // Check for out of bounds
    static bool oobPlot = false;
    if (position.y < 0 || position.y >= CHUNK_SIZE || position.x < 0 || position.x >= CHUNK_SIZE) {
        return oobPlot;
    }

    // Get the tile
    return m_structurePlots.at(position.y * CHUNK_SIZE + position.x);
}

void TileChunk::clearPlots() { m_structurePlots.fill(false); }

Tile* TileChunk::getTile(const sf::Vector2i& position)
{
    // Check for out of bounds
    static Tile e;
    if (position.y < 0 || position.y >= CHUNK_SIZE || position.x < 0 || position.x >= CHUNK_SIZE) {
        return &e;
    }

    // Get the tile
    return &m_tiles.at(position.y * CHUNK_SIZE + position.x);
}

Tile* TileChunk::getGlobalTile(const sf::Vector2i& position)
{
    return p_chunkManager->getTile(toGlobalTilePosition(chunkPosition, position));
}

//   ;

void TileChunk::updateAllTiles()
{
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            updateTile({x, y});
        }
    }
}

void TileChunk::setTile(const sf::Vector2i& position, TileType type) { getTile(position)->type = type; }

GridMap::GridMap()
{
    for (int i = 0; i < CHUNK_SIZE + 1; i++) {
        addGridLine(&m_grid, tileToScreenPosition({0, i}), tileToScreenPosition({CHUNK_SIZE, i}));
        addGridLine(&m_grid, tileToScreenPosition({i, 0}), tileToScreenPosition({i, CHUNK_SIZE}));
    }
}

void GridMap::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    states.transform *= getTransform();
    window.draw(m_grid.data(), m_grid.size(), sf::Lines, states);
}
