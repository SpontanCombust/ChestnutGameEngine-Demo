#pragma once

#include <chestnut/engine/maths/vector2.hpp>
#include <chestnut/engine/serialization/serializers_math.hpp>
#include <nlohmann/json.hpp>

using namespace chestnut::engine;

struct PlayerComponent
{
    vec2f movementDirection;
};


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerComponent,
    movementDirection
)