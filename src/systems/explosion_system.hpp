#pragma once

#include <chestnut/ecs/entity_query.hpp>
#include <chestnut/engine/ecs_impl/logic_system.hpp>
#include <chestnut/engine/ecs_impl/events/collision_event.hpp>

#include "events/explosion_event.hpp"

#include <vector>


using namespace chestnut::ecs;
using namespace chestnut::engine;

class ExplosionSystem : public ILogicSystem
{
private:
    CEntityQuery *m_explosionQuery;
    // A workaround for the fact that when a collision event happes
    // and an explosion hits a brittle block, said block should be destroyed
    // Currently the issue of entities being able to get destroyed while 
    // they're being iterated over in the query has not been fixed yet
    // The event system is kinda terrible atm...
    std::vector<entityid_t> m_blockEntsToBeDestroyed;

public:
    void onAttach() override;
    void onDetach() override;

    void update(float dt) override;

    event_function onExplosionStart(const ExplosionEvent& e);
    event_function onExplosionCollision(const SCollisionEvent& e);
};