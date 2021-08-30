#include "StructureRegistry.h"

StructureDef& StructureDef::giveVarity(int variations)
{
    this->variations = variations;
    return *this;
}

StructureDef& StructureDef::loadGuiTexture(const std::string& textureName)
{
    static std::string path = "data/Textures/GUI/";
    guiTexture.loadFromFile(path + textureName);
    return *this;
}

StructureDef& StructureDef::setConstructionType(ConstructionType constructionType)
{
    this->constructionType = constructionType;
    return *this;
}

StructureRegistry::StructureRegistry()
{
    registerStructure(StructureType::FirTree, "Fir Tree", {1, 2}, 0, {1, 1},
                      VairantType::Random, StructurePlacement::Land)
        .giveVarity(7);

    registerStructure(StructureType::MudWall, "Mud Wall", {1, 3}, 2, {1, 1},
                      VairantType::Neighbour, StructurePlacement::Land)
        .loadGuiTexture("MudWall.png")
        .setConstructionType(ConstructionType::DynamicPath);

    registerStructure(StructureType::StoneWall, "Stone Wall", {1, 4}, 5, {1, 1},
                      VairantType::Neighbour, StructurePlacement::Land)
        .loadGuiTexture("StoneWall.png")
        .setConstructionType(ConstructionType::DynamicPath);

    registerStructure(StructureType::WoodWall, "Wood Wall", {1, 2}, 9, {1, 1},
                      VairantType::Neighbour, StructurePlacement::Land)
        .loadGuiTexture("WoodWall.png")
        .setConstructionType(ConstructionType::DynamicPath);

    registerStructure(StructureType::Base, "Base Base", {2, 4}, 11, {2, 2},
                      VairantType::None, StructurePlacement::Land);
}

StructureRegistry& StructureRegistry::instance()
{
    static StructureRegistry reg;
    return reg;
}

const StructureDef& StructureRegistry::getStructure(StructureType type)
{
    return m_structures[(size_t)type];
}

StructureDef&
StructureRegistry::registerStructure(StructureType type, const std::string& name,
                                     const sf::Vector2f& textureSize, int textureIndex,
                                     const sf::Vector2i& baseSize, VairantType variance,
                                     StructurePlacement placement)
{
    StructureDef def;
    def.name = name;

    def.textureSize = textureSize;
    def.textureIndex = textureIndex;

    def.baseSize = baseSize;

    def.variantType = variance;
    def.placement = placement;

    m_structures[(size_t)type] = def;
    return m_structures[(size_t)type];
}
