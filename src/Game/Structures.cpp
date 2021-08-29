#include "Structures.h"

#include <array>
#include <iostream>

std::array<StructureDef, (size_t)StructureType::NUM_TYPES> structures;

void registerStructures()
{
    static bool registered = false;

    if (!registered) {
        registerStructure(StructureType::FirTree, {1, 2}, 0, {1, 1}, VairantType::Random,
                          StructurePlacement::Land)
            .giveVarity(7);

        registerStructure(StructureType::MudWall, {1, 3}, 2, {1, 1},
                          VairantType::Neighbour, StructurePlacement::Land);

        registerStructure(StructureType::StoneWall, {1, 4}, 5, {1, 1},
                          VairantType::Neighbour, StructurePlacement::Land);

        registerStructure(StructureType::WoodWall, {1, 2}, 9, {1, 1},
                          VairantType::Neighbour, StructurePlacement::Land);

        registerStructure(StructureType::Base, {2, 4}, 11, {2, 2}, VairantType::None,
                          StructurePlacement::Land);

        registered = true;
    }
}

StructureDef& registerStructure(StructureType type, const sf::Vector2f& textureSize,
                                int textureIndex, const sf::Vector2i& baseSize,
                                VairantType variance, StructurePlacement placement)
{
    StructureDef def;

    def.textureSize = textureSize;
    def.textureIndex = textureIndex;

    def.baseSize = baseSize;

    def.variantType = variance;
    def.placement = placement;

    structures[(size_t)type] = def;
    return structures[(size_t)type];
}

const StructureDef& getStructure(StructureType type) { return structures[(size_t)type]; }

StructureDef& StructureDef::giveVarity(int variations)
{
    this->variations = variations;
    return *this;
}
