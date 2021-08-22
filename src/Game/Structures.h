
#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>

enum class VairantType {
    None = 0,
    Neighbour = 1,
    Random = 2,
};

enum class StructureType {
    FirTree = 0,
    MudWall = 1,
    StoneWall = 2,

    NUM_TYPES
};

enum class StructurePlacement {
    Land = 0,
    Water = 1,
    WatersEdge = 2,
};

struct StructureDef {
    sf::Vector2f size;
    int textureIndex;

    VairantType variantType;
    int variations; // For Random variation

    StructurePlacement placement;

    // Builder functions
    StructureDef& giveVarity(int variations);
};

struct Structure {
    StructureType type;
    int variant = 0;
};

void registerStructures();
StructureDef& registerStructure(StructureType type, const sf::Vector2f& size,
                                int textureIndex, VairantType variance,
                                StructurePlacement placement);

const StructureDef& getStructure(StructureType type);