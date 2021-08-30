#include "ScreenGame.h"
#include "Common.h"
#include "ScreenMainMenu.h"
#include "Tiles.h"
#include <cmath>
#include <imgui_sfml/imgui-SFML.h>
#include <imgui_sfml/imgui.h>
#include <iostream>
#include <unordered_set>

ScreenGame::ScreenGame(ScreenManager* stack)
    : Screen(stack)
{
    m_selectionTexture.loadFromFile("data/Textures/Selection.png");
    m_selectionQuadTexture.loadFromFile("data/Textures/SelectionQuad.png");
    m_tileCorners.loadFromFile("data/Textures/Corners.png");

    m_selectionRect.setSize({TILE_WIDTH, TILE_HEIGHT});

    m_tileManager.initWorld();
}

void ScreenGame::onInput(const Keyboard& keyboard, const sf::RenderWindow& window)
{
    m_camera.onInput(keyboard, window);

    if (!ImGui::GetIO().WantCaptureMouse) {
        auto mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePosition);

        sf::Vector2i cell = {(int)worldPos.x / (int)TILE_WIDTH,
                             (int)worldPos.y / (int)TILE_HEIGHT};
        sf::Vector2i offset = {(int)worldPos.x % (int)TILE_WIDTH,
                               (int)worldPos.y % (int)TILE_HEIGHT};

        m_selectedTile = {
            (cell.y - CHUNK_SIZE) + (cell.x - CHUNK_SIZE),
            (cell.y - CHUNK_SIZE) - (cell.x - CHUNK_SIZE),
        };

        // clang-format off
        if (offset.x >= 0 && 
            offset.y >= 0 && 
            offset.x < (int)m_tileCorners.getSize().x && 
            offset.y < (int)m_tileCorners.getSize().y)
        {
            sf::Color colour = m_tileCorners.getPixel(offset.x, offset.y);
                 if (colour == sf::Color::Red   )   m_selectedTile.x--;
            else if (colour == sf::Color::Green )   m_selectedTile.y++;
            else if (colour == sf::Color::Blue  )   m_selectedTile.y--;
            else if (colour == sf::Color::White )   m_selectedTile.x++;
        }

        m_tileManager.setCurrentlySelectedTile(m_selectedTile);

        // clang-format on
    }

    if (m_isConstructing) {
        if (xDistGreater(m_editStartPosition, m_editEndPosition)) {
            m_editPivotPoint.x = m_selectedTile.x;
            m_editPivotPoint.y = m_editStartPosition.y;
        }
        else {
            m_editPivotPoint.x = m_editStartPosition.x;
            m_editPivotPoint.y = m_selectedTile.y;
        }
        m_editEndPosition = m_selectedTile;
    }
}

void ScreenGame::onGUI()
{
    float panelSize = 256;
    ImGui::SetNextWindowBgAlpha(1);
    ImGui::SetNextWindowPos({1600 - panelSize, 0});
    ImGui::SetNextWindowSize({panelSize, 900});
    if (ImGui::Begin("Panel", nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
        ImGui::Text("Tile: %d %d", m_selectedTile.x, m_selectedTile.y);
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Performance %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
                    io.Framerate);
        if (ImGui::Button("Regen world")) {
            m_tileManager.regenerateTerrain();
        }

        onConstructionGUI(m_currConstruction);

        ImGui::End();
    }
}

void ScreenGame::onEvent(const sf::Event& e)
{
    auto tryPlaceStructure = [&](const sf::Vector2i& tilePosition,
                                 StructureType structure) {
        if (m_tileManager.canPlaceStructure(tilePosition, structure)) {
            m_tileManager.placeStructure(structure, tilePosition);
        }
    };

    m_camera.onEvent(e);
    if (!ImGui::GetIO().WantCaptureMouse) {
        if (e.type == sf::Event::MouseButtonPressed) {
            m_isConstructing = true;
            m_editStartPosition = m_selectedTile;
            m_editEndPosition = m_selectedTile;
        }
        else if (e.type == sf::Event::MouseButtonReleased) {
            if (m_currConstruction.action == CurrentConstruction::Action::Constructing) {
                auto structure = m_currConstruction.strType;

                const StructureDef& def = StructureRegistry::instance().getStructure(
                    m_currConstruction.strType);

                if (def.constructionType == ConstructionType::DynamicPath) {
                    forEachLSection(m_editStartPosition, m_editPivotPoint,
                                    m_editEndPosition,
                                    [&](const sf::Vector2i& tilePosition) {
                                        tryPlaceStructure(tilePosition, structure);
                                    });
                }
                else if (def.constructionType == ConstructionType::DynamicQuad) {
                    forEachQuadSection(m_editStartPosition, m_editEndPosition,
                                       [&](const sf::Vector2i& tilePosition) {
                                           tryPlaceStructure(tilePosition, structure);
                                       });
                }
                else if (def.constructionType == ConstructionType::Quad) {
                    for (int y = 0; y < def.baseSize.y; y++) {
                        for (int x = 0; x < def.baseSize.x; x++) {
                            if (m_tileManager.canPlaceStructure(m_selectedTile,
                                                                structure)) {
                                m_tileManager.placeStructure(structure, m_selectedTile);
                            }
                        }
                    }
                }
            }
            else if (m_currConstruction.action == CurrentConstruction::Action::Selling) {
                if (m_tileManager.isStructureAt(m_selectedTile)) {
                    m_tileManager.removeStructure(m_selectedTile);
                }
            }
            m_isConstructing = false;
            // m_currConstruction.action = CurrentConstruction::Action::None;
        }
    }
}

void ScreenGame::onUpdate(const sf::Time& dt) {}

void ScreenGame::onFixedUpdate(const sf::Time& dt) {}

void ScreenGame::onRender(sf::RenderWindow* window)
{
    m_camera.setViewToCamera(*window);

    // Render the tile map
    m_tileManager.showDetail = m_camera.zoomLevel < 2;
    m_tileManager.draw(window);

    // Render the selected tile
    m_selectionRect.setTexture(&m_selectionTexture);
    m_selectionRect.setPosition(tileToScreenPosition(m_selectedTile));
    window->draw(m_selectionRect);

    // If the player is constructing, then render where they want to construct
    m_selectionRect.setTexture(&m_selectionQuadTexture);
    auto renderPlacementRect = [&](const sf::Vector2i& tilePosition,
                                   StructureType strType) {
        if (m_tileManager.canPlaceStructure(tilePosition, strType)) {
            m_selectionRect.setFillColor(sf::Color::Green);
        }
        else {
            m_selectionRect.setFillColor(sf::Color::Red);
        }
        m_selectionRect.setPosition(tileToScreenPosition(tilePosition));
        window->draw(m_selectionRect);
    };

    if (m_currConstruction.action == CurrentConstruction::Action::Constructing &&
        m_isConstructing) {
        StructureType strType = m_currConstruction.strType;
        const StructureDef& def = StructureRegistry::instance().getStructure(strType);

        if (def.constructionType == ConstructionType::DynamicPath) {

            forEachLSection(m_editStartPosition, m_editPivotPoint, m_editEndPosition,
                            [&](const sf::Vector2i& tilePosition) {
                                renderPlacementRect(tilePosition, strType);
                            });
        }
        else if (def.constructionType == ConstructionType::DynamicQuad) {
            forEachQuadSection(m_editStartPosition, m_editEndPosition,
                               [&](const sf::Vector2i& tilePosition) {
                                   renderPlacementRect(tilePosition, strType);
                               });
        }
        else if (def.constructionType == ConstructionType::Quad)
            for (int y = 0; y < def.baseSize.y; y++) {
                for (int x = 0; x < def.baseSize.x; x++) {
                    m_selectionRect.setPosition(
                        tileToScreenPosition(m_selectedTile - sf::Vector2i{x, y}));
                    window->draw(m_selectionRect);
                }
            }
    }
    else if (m_currConstruction.action == CurrentConstruction::Action::Selling) {
        if (m_tileManager.isStructureAt(m_selectedTile)) {
        }
    }
}
