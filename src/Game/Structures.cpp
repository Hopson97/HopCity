#include "Structures.h"

#include <array>

std::array<StructureDef, (size_t)StructureType::NUM_TYPES> structures;

void registerStructures()
{
    static bool registered = false;

    if (!registered) {
        registerStructure(StructureType::FirTree, {1, 2}, 0, VairantType::None);
        registerStructure(StructureType::MudWall, {1, 2}, 2, VairantType::Neighbour);
        registerStructure(StructureType::StoneWall, {1, 4}, 4, VairantType::Neighbour);
    }
}

void registerStructure(StructureType type, const sf::Vector2f& size, int textureIndex,
                       VairantType variance)
{
    StructureDef def;
    def.size = size;
    def.variantType = variance;
    def.textureIndex = textureIndex;

    structures[(size_t)type] = def;
}

const StructureDef& getStructure(StructureType type) { return structures[(size_t)type]; }