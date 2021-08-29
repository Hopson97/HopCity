#pragma once

#include "Structures.h"

struct CurrentConstruction {
    StructureType strType = StructureType::MudWall;
};

void onConstructionGUI(CurrentConstruction& currCon);