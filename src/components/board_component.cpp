#include "board_component.hpp"

#include <chestnut/engine/main/engine.hpp>

using namespace chestnut::engine;
using namespace chestnut::ecs;

entityid_t fetchBoardEntity()
{
    return CEngine::getInstance()
    .getEntityWorld()
    .findEntities([](const CEntitySignature& sign) {
        return sign.has<BoardComponent>();
    })[0];
}

Board& getBoard()
{
    static entityid_t boardEnt = fetchBoardEntity(); // ran only once

    return CEngine::getInstance()
    .getEntityWorld()
    .getComponent<BoardComponent>(boardEnt)->board;
}