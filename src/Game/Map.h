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

constexpr int CHUNK_SIZE = 16;

struct TileChunk : public sf::Drawable, private sf::Transformable {
    std::vector<Tile> tiles;
    std::vector<sf::Vertex> m_tileVerts;
    sf::Texture* tileTextures;

    sf::Vector2i chunkPosition;

    void init(const sf::Vector2i& position, int worldSize);

    void draw(sf::RenderTarget& window,
              sf::RenderStates states = sf::RenderStates::Default) const override;

    void updateTile(const sf::Vector2i& position);

    Tile* getTile(const sf::Vector2i& position);
};

struct TileChunkManager {
    std::vector<TileChunk> tilechunks;
    sf::Texture tileTextures;
    void initChunks();
};

/*
struct Map {
  public:
    Map(int worldSize);

    void initWorld();

    void regenerateTerrain();
    void setTile(const sf::Vector2i& position, TileType type);
    void draw(sf::RenderWindow* target);
    void placeStructure(StructureType type, const sf::Vector2i& position);
    Tile* getTile(const sf::Vector2i& position);

    bool showDetail;

  private:
    void updateTile(const sf::Vector2i& position);

    sf::Texture m_tileTextures;
    std::vector<Tile> m_tiles;
    std::vector<sf::Vertex> m_grid;
    std::vector<sf::Vertex> m_tileVerts;

    std::unordered_map<sf::Vector2i, Structure, Vec2hash> m_structures;
    std::set<sf::Vector2i, Vec2Compare> sorted;

    sf::RectangleShape m_structureRect;
    sf::Texture m_structureMap;

    int m_worldSize;
};*/