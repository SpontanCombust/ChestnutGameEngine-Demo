#pragma once

#include <chestnut/engine/ecs_impl/logic_system.hpp>
#include <chestnut/ecs/entity_query.hpp>


using namespace chestnut::ecs;
using namespace chestnut::engine;

class BombSystem : public ILogicSystem
{
private:
    CEntityQuery *m_bombQuery;

public:
    void onAttach() override;
    void onDetach() override;

    void update(float dt) override;
};