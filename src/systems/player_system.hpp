#pragma once

#include <chestnut/engine/ecs_impl/logic_system.hpp>
#include <chestnut/ecs/types.hpp>
#include <SDL_events.h>

using namespace chestnut::engine;
using namespace chestnut::ecs;

class PlayerSystem : public ILogicSystem
{
private:
    entityid_t m_playerEnt;

public:
    void onAttach() override;
    void onDetach() override;

    void update(float dt) override;

    event_function onInputDetected(const SDL_KeyboardEvent& e);

private:
    void placeBomb();
};