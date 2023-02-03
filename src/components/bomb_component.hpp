#pragma once

#include <nlohmann/json.hpp>

struct BombComponent
{
    float fuseLeftSec = 0.f;
    float explosionDuration = 0.f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BombComponent,
    fuseLeftSec,
    explosionDuration
)