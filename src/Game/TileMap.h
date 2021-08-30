#pragma once

#include "Registry/StructureRegistry.h"
#include "Registry/TileRegistry.h"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <unordered_map>

class TileMap;

class GridMap : public sf::Drawable, public sf::Transformable {
  public:
    GridMap();
    void draw(sf::RenderTarget& window,
              sf::RenderStates states = sf::RenderStates::Default) const override;

  private:
    std::vector<sf::Vertex> m_grid;
};

class TileChunk : public sf::Drawable, private sf::Transformable {
  public:
    TileChunk(const sf::Vector2i& position, TileMap* chunkManager);

    void draw(sf::RenderTarget& window,
              sf::RenderStates states = sf::RenderStates::Default) const override;

    void setTile(const sf::Vector2i& position, TileType type);
    void updateAllTiles();
    void updateTile(const sf::Vector2i& position);

    bool& getStructurePlot(const sf::Vector2i& position);
    void clearPlots();

    Tile* getTile(const sf::Vector2i& position);

    sf::Vector2i chunkPosition;
    TileMap* p_chunkManager = nullptr;

  private:
    Tile* getGlobalTile(const sf::Vector2i& position);

    std::vector<sf::Vertex> m_tileVerts;

    std::array<Tile, CHUNK_SIZE * CHUNK_SIZE> m_tiles;
    std::array<bool, CHUNK_SIZE * CHUNK_SIZE> m_structurePlots{false};
};

class TileMap {
  public:
    void initWorld();
    TileChunk& addChunk(const sf::Vector2i& chunkPos);

    void regenerateTerrain();
    void setTile(const sf::Vector2i& position, TileType type);
    void draw(sf::RenderWindow* target);
    Tile* getTile(const sf::Vector2i& position);

    bool showDetail;

    bool canPlaceStructure(const sf::Vector2i& basePosition, StructureType type);

    bool isStructureAt(const sf::Vector2i& tilePosition)
    {
        return getStructurePlot(tilePosition);
    }
    bool& getStructurePlot(const sf::Vector2i& position);

  private:
    sf::Texture m_tileTextures;
    std::unordered_map<sf::Vector2i, TileChunk, Vec2hash> m_chunks;

    GridMap m_gridMap;
};