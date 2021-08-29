
#pragma once

#include "Common.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstdint>

enum class ConstructionType {
    None = 0,
    DynamicPath = 1,
    DynamicQuad = 2,

    Quad = 3,
};

enum class StructureType {
    FirTree = 0,
    MudWall = 1,
    StoneWall = 2,
    WoodWall = 3,

    Base,

    NUM_TYPES
};

enum class StructurePlacement {
    Land = 0,
    Water = 1,
    WatersEdge = 2,
};

struct StructureDef {
    std::string name;

    sf::Vector2f textureSize;
    sf::Vector2i baseSize;
    int textureIndex = 0;

    VairantType variantType = VairantType::None;

    StructurePlacement placement = StructurePlacement::Land;

    // Only some have this
    int goldCost = 0;

    int variations = 0; // For Random variation

    sf::Texture guiTexture;
    ConstructionType constructionType = ConstructionType::None;

    // Builder functions
    StructureDef& giveVarity(int variations);
    StructureDef& loadGuiTexture(const std::string& textureName);
    StructureDef& setConstructionType(ConstructionType type);
};

struct Structure {
    StructureType type;
    int variant = 0;
};

class StructureRegistry {
  public:
    static StructureRegistry& instance();

    const StructureDef& getStructure(StructureType type);

  private:
    StructureRegistry();

    StructureDef& registerStructure(StructureType type, const std::string& name,
                                    const sf::Vector2f& textureSize, int textureIndex,
                                    const sf::Vector2i& baseSize, VairantType variance,
                                    StructurePlacement placement);

    std::array<StructureDef, (size_t)StructureType::NUM_TYPES> m_structures;
};
