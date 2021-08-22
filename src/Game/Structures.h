
#pragma once

#include "Common.h"
#include <SFML/System/Vector2.hpp>
#include <cstdint>

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
    int textureIndex = 0;

    VairantType variantType = VairantType::None;
    int variations = 0; // For Random variation

    StructurePlacement placement = StructurePlacement::Land;

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