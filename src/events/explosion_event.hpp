#pragma once

#include <chestnut/engine/maths/vector2.hpp>


using namespace chestnut::engine;

struct ExplosionEvent
{
    vec2f position;
    float explosionLifetime;
};