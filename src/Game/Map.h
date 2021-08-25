#pragma once

#include "../Animation.h"
#include "Structures.h"
#include "Tiles.h"
#include "unordered_map"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <set>

class TileChunkManager;

class TileChunk : public sf::Drawable, private sf::Transformable {
  public:
    TileChunk(const sf::Vector2i& position, TileChunkManager* chunkManager);
    void generateTerrain(int seed);

    void draw(sf::RenderTarget& window,
              sf::RenderStates states = sf::RenderStates::Default) const override;

    void updateTile(const sf::Vector2i& position);

    Tile* getTile(const sf::Vector2i& position);

  private:
    Tile* getGlobalTile(const sf::Vector2i& position);

    std::vector<Tile> m_tiles;
    std::vector<sf::Vertex> m_tileVerts;

    sf::Vector2i m_chunkPosition;

    TileChunkManager* mp_chunkManager = nullptr;
};

class TileChunkManager {
  public:
    void initWorld();
    void addChunk(const sf::Vector2i& chunkPos);

    void regenerateTerrain();
    void setTile(const sf::Vector2i& position, TileType type);
    void draw(sf::RenderWindow* target);
    void placeStructure(StructureType type, const sf::Vector2i& position);
    Tile* getTile(const sf::Vector2i& position);

    bool showDetail;

  private:
    sf::Texture m_tileTextures;
    std::unordered_map<sf::Vector2i, TileChunk, Vec2hash> m_chunks;
    std::vector<sf::Vertex> m_grid;

    std::unordered_map<sf::Vector2i, Structure, Vec2hash> m_structures;
    std::set<sf::Vector2i, Vec2Compare> sorted;

    sf::RectangleShape m_structureRect;
    sf::Texture m_structureMap;

    int m_seed = 52323;
};