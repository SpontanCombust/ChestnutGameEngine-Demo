#include "bomb_system.hpp"

#include "components/bomb_component.hpp"
#include "components/explosion_emitter_component.hpp"
#include "events/explosion_event.hpp"

#include <chestnut/engine/main/engine.hpp>
#include <chestnut/engine/ecs_impl/components/transform2d_component.hpp>


using namespace chestnut::ecs;
using namespace chestnut::engine;

void BombSystem::onAttach()
{
    m_bombQuery = CEngine::getInstance().getEntityWorld().createQuery(
        makeEntitySignature<BombComponent>()
    );
}

void BombSystem::onDetach()
{
    CEngine::getInstance().getEntityWorld().destroyQuery(m_bombQuery);
}

void BombSystem::update(float dt)
{
    auto& w = CEngine::getInstance().getEntityWorld();

    w.queryEntities(m_bombQuery);
    auto it = m_bombQuery->begin<BombComponent>();
    auto end = m_bombQuery->end<BombComponent>();

    for(; it != end; ++it)
    {
        auto& [b] = *it;

        b.fuseLeftSec -= dt;
        if(b.fuseLeftSec < 0.f)
        {
            auto bombPos = w.getComponent<CTransform2DComponent>(it.entityId())->position;
            w.destroyEntity(it.entityId());

            CEngine::getInstance()
            .getEventManager()
            .raiseEvent(ExplosionEvent{
                bombPos,
                b.explosionDuration
            });
        }
    }
}