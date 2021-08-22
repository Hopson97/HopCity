
#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>

enum class VairantType {
    None,
    Neighbour,

};

enum class StructureType {
    FirTree,
    MudWall,
    StoneWall,

    NUM_TYPES
};

struct StructureDef {
    sf::Vector2f size;
    int textureIndex;
    VairantType variantType;
};

struct Structure {
    StructureType type;
    int variant = 0;
};

void registerStructures();
void registerStructure(StructureType type, const sf::Vector2f& size, int textureIndex,
                       VairantType variance);
const StructureDef& getStructure(StructureType type);