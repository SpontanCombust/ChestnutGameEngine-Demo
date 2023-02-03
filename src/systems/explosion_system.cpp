#include "explosion_system.hpp"

#include <chestnut/ecs/entity_query.hpp>
#include <chestnut/engine/main/engine.hpp>
#include <chestnut/engine/ecs_impl/logic_system.hpp>
#include <chestnut/engine/ecs_impl/components/transform2d_component.hpp>
#include <chestnut/engine/ecs_impl/components/model2d_component.hpp>
#include <chestnut/engine/ecs_impl/components/sprite_component.hpp>
#include <chestnut/engine/ecs_impl/components/animation2d_component.hpp>
#include <chestnut/engine/ecs_impl/components/collision2d_component.hpp>
#include <chestnut/engine/resources/texture2d_resource.hpp>
#include <chestnut/engine/resources/animation2d_resource.hpp>
#include <chestnut/engine/resources/entity_template_resource.hpp>
#include <chestnut/engine/misc/utility_functions.hpp>

#include "components/explosion_emitter_component.hpp"
#include "components/explosion_component.hpp"
#include "components/player_component.hpp"
#include "components/block_component.hpp"
#include "components/board_component.hpp"
#include "events/explosion_event.hpp"
#include "game_params.hpp"


void ExplosionSystem::onAttach()
{
    m_explosionQuery = CEngine::getInstance().getEntityWorld().createQuery(
        makeEntitySignature<ExplosionEmitterComponent>()
    );

    addEventListener(
        "onExplosionStart", new CEventListener<ExplosionEvent>(
        std::bind(&ExplosionSystem::onExplosionStart, this, std::placeholders::_1)   
    ));
    addEventListener(
        "onExplosionCollision", new CEventListener<SCollisionEvent>(
        std::bind(&ExplosionSystem::onExplosionCollision, this, std::placeholders::_1),
        [](const SCollisionEvent& e) -> bool {
            const auto& w = CEngine::getInstance().getEntityWorld();
            return w.hasComponent<ExplosionComponent>(e.entity1) || w.hasComponent<ExplosionComponent>(e.entity2);
        }
    ));
}

void ExplosionSystem::onDetach()
{
    CEngine::getInstance().getEntityWorld().destroyQuery(m_explosionQuery);
}

entityid_t createExplosionFieldEntity()
{
    static auto explosionTemplate = CEngine::getInstance()
    .getResourceManager()
    .getOrLoadResource<CEntityTemplateResource>(assetPathToAbsolute("explosion.ent")).value();

    return explosionTemplate->createEntity().value();
}

event_function ExplosionSystem::onExplosionStart(const ExplosionEvent& e)
{
    auto& w = CEngine::getInstance().getEntityWorld();
    auto& board = getBoard();   

    entityid_t explosionEmitter = w.createEntity();
    w.createComponent<CTransform2DComponent>(explosionEmitter)->position = e.position;
    auto emitterComp = w.createComponent<ExplosionEmitterComponent>(explosionEmitter);
    emitterComp->lifetimeLeft = e.explosionLifetime;


    vec2f explPos;
    entityid_t explEnt;

    explPos = e.position;
    explEnt = createExplosionFieldEntity();
    w.getComponent<CTransform2DComponent>(explEnt)->position = e.position;
    w.getComponent<CAnimation2DComponent>(explEnt)->playAnimation("flame_center");
    emitterComp->explosionEnts[0] = explEnt;

    explPos = {e.position.x - BLOCK_SIZE, e.position.y};
    if(board.getBlockTypeAtPos(explPos).value_or(BlockType::VOID) != BlockType::SOLID)
    {
        explEnt = createExplosionFieldEntity();
        w.getComponent<CTransform2DComponent>(explEnt)->position = explPos;
        w.getComponent<CAnimation2DComponent>(explEnt)->playAnimation("flame_left");
        emitterComp->explosionEnts[1] = explEnt;
    }

    explPos = {e.position.x + BLOCK_SIZE, e.position.y};
    if(board.getBlockTypeAtPos(explPos).value_or(BlockType::VOID) != BlockType::SOLID)
    {
        explEnt = createExplosionFieldEntity();
        w.getComponent<CTransform2DComponent>(explEnt)->position = explPos;
        w.getComponent<CAnimation2DComponent>(explEnt)->playAnimation("flame_right");
        emitterComp->explosionEnts[2] = explEnt;
    }

    explPos = {e.position.x, e.position.y - BLOCK_SIZE};
    if(board.getBlockTypeAtPos(explPos).value_or(BlockType::VOID) != BlockType::SOLID)
    {
        explEnt = createExplosionFieldEntity();
        w.getComponent<CTransform2DComponent>(explEnt)->position = explPos;
        w.getComponent<CAnimation2DComponent>(explEnt)->playAnimation("flame_up");
        emitterComp->explosionEnts[3] = explEnt;
    }

    explPos = {e.position.x, e.position.y + BLOCK_SIZE};
    if(board.getBlockTypeAtPos(explPos).value_or(BlockType::VOID) != BlockType::SOLID)
    {
        explEnt = createExplosionFieldEntity();
        w.getComponent<CTransform2DComponent>(explEnt)->position = explPos;
        w.getComponent<CAnimation2DComponent>(explEnt)->playAnimation("flame_down");
        emitterComp->explosionEnts[4] = explEnt;
    }
}

event_function ExplosionSystem::onExplosionCollision(const SCollisionEvent& e) 
{
    auto& w = CEngine::getInstance().getEntityWorld();

    entityid_t otherCollider = w.hasComponent<ExplosionComponent>(e.entity1) ? e.entity2 : e.entity1; 

    if(w.hasComponent<PlayerComponent>(otherCollider))
    {
        messageBoxInfo("GAME OVER\nPlayer was killed by an explosion");
    }
    else if(w.hasComponent<BlockComponent>(otherCollider))
    {
        if(w.getComponent<BlockComponent>(otherCollider)->breakable)
        {
            m_blockEntsToBeDestroyed.push_back(otherCollider);
        }
    }
}

void ExplosionSystem::update(float dt)
{
    auto& w = CEngine::getInstance().getEntityWorld();

    w.queryEntities(m_explosionQuery);
    auto it = m_explosionQuery->begin<ExplosionEmitterComponent>();
    auto end = m_explosionQuery->end<ExplosionEmitterComponent>();

    for(; it != end; ++it)
    {
        auto& [expl] = *it;

        expl.lifetimeLeft -= dt;
        if(expl.lifetimeLeft < 0.f)
        {
            auto& w = CEngine::getInstance().getEntityWorld();

            w.destroyEntity(expl.explosionEnts[0]);
            w.destroyEntity(expl.explosionEnts[1]);
            w.destroyEntity(expl.explosionEnts[2]);
            w.destroyEntity(expl.explosionEnts[3]);
            w.destroyEntity(expl.explosionEnts[4]);

            w.destroyEntity(it.entityId());
        }
    }

    for(auto ent : m_blockEntsToBeDestroyed)
    {
        w.destroyEntity(ent);
    }

    m_blockEntsToBeDestroyed.clear();
}