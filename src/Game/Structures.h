
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

enum class StructurePlacement { Land, Water, WatersEdge };

struct StructureDef {
    sf::Vector2f size;
    int textureIndex;
    VairantType variantType;
    StructurePlacement placement;
};

struct Structure {
    StructureType type;
    int variant = 0;
};

void registerStructures();
void registerStructure(StructureType type, const sf::Vector2f& size, int textureIndex,
                       VairantType variance, StructurePlacement placement);
const StructureDef& getStructure(StructureType type);