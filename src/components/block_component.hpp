#pragma once

#include <nlohmann/json.hpp>

struct BlockComponent
{
    bool breakable = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BlockComponent,
    breakable
)