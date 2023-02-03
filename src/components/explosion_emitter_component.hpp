#pragma once

#include <chestnut/ecs/types.hpp>

#include <algorithm>


using namespace chestnut::ecs;

struct ExplosionEmitterComponent
{
    float lifetimeLeft = 0.f;
    entityid_t explosionEnts[5];

    ExplosionEmitterComponent()
    {
        std::fill_n(explosionEnts, 5, ENTITY_ID_INVALID);
    }
};