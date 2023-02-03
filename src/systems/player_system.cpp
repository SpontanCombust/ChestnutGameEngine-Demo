#include "player_system.hpp"

#include "components/player_component.hpp"
#include "components/board_component.hpp"

#include <chestnut/engine/main/engine.hpp>
#include <chestnut/engine/ecs_impl/components/kinematics2d_component.hpp>
#include <chestnut/engine/ecs_impl/systems/simple2d_rendering_system.hpp>
#include <chestnut/engine/ecs_impl/components/transform2d_component.hpp>
#include <chestnut/engine/ecs_impl/components/animation2d_component.hpp>
#include <chestnut/engine/resources/entity_template_resource.hpp>
#include <chestnut/engine/misc/utility_functions.hpp>
#include <chestnut/engine/maths/vector_cast.hpp>

static const float PLAYER_SPEED_MULT = 100.f;
static std::shared_ptr<CEntityTemplateResource> bombEntTemplate;

void PlayerSystem::onAttach() 
{
    auto& eng = CEngine::getInstance();
    auto& w = eng.getEntityWorld();
    
    m_playerEnt = w.findEntities([](const CEntitySignature& sign) {
        return sign.has<PlayerComponent>();
    })[0];

    bombEntTemplate = CEngine::getInstance()
    .getResourceManager()
    .getOrLoadResource<CEntityTemplateResource>(
        assetPathToAbsolute("bomb.ent")
    ).value();

    this->addEventListener("onInputDetected", new CEventListener<SDL_KeyboardEvent>(
        std::bind(&PlayerSystem::onInputDetected, this, std::placeholders::_1)
    ));
}

void PlayerSystem::onDetach() 
{

}

void PlayerSystem::update(float dt) 
{
    auto& camera = CEngine::getInstance().getRenderingSystem<CSimple2DRenderingSystem>()->getCamera();
    auto playerPos = CEngine::getInstance().getEntityWorld().getComponent<CTransform2DComponent>(m_playerEnt)->position;
    auto winSize = vecCastType<float>(CEngine::getInstance().getWindow().getSize());

    camera.m_zoom = 3.0f;
    camera.m_position = playerPos;
}



void PlayerSystem::placeBomb() 
{
    auto& board = getBoard();

    auto bombEnt = bombEntTemplate->createEntity().value();

    vec2f playerPos = CEngine::getInstance()
    .getEntityWorld()
    .getComponent<CTransform2DComponent>(m_playerEnt)->position;

    CEngine::getInstance()
    .getEntityWorld()
    .getComponent<CTransform2DComponent>(bombEnt)->position = board.getPositionShiftedToBlockCenter(playerPos);
}

event_function PlayerSystem::onInputDetected(const SDL_KeyboardEvent& e) 
{
    auto& w = CEngine::getInstance().getEntityWorld();

    auto& playerComp = w.getComponent<PlayerComponent>(m_playerEnt).get();
    auto& kinemComp = w.getComponent<CKinematics2DComponent>(m_playerEnt).get();
    auto& animComp = w.getComponent<CAnimation2DComponent>(m_playerEnt).get();

    if(e.type == SDL_KEYDOWN)
    {
        switch(e.keysym.sym)
        {
        case SDLK_UP:
            playerComp.movementDirection.y = -1.f;
            if(animComp.currentAnimName != "walk_up" && playerComp.movementDirection.x == 0.f) 
                animComp.playAnimation("walk_up");
            break;
        case SDLK_DOWN:
            playerComp.movementDirection.y = 1.f;
            if(animComp.currentAnimName != "walk_down" && playerComp.movementDirection.x == 0.f) 
                animComp.playAnimation("walk_down");
            break;
        case SDLK_LEFT:
            playerComp.movementDirection.x = -1.f;
            if(animComp.currentAnimName != "walk_left" && playerComp.movementDirection.y == 0.f) 
                animComp.playAnimation("walk_left");
            break;
        case SDLK_RIGHT:
            playerComp.movementDirection.x = 1.f;
            if(animComp.currentAnimName != "walk_right" && playerComp.movementDirection.y == 0.f) 
                animComp.playAnimation("walk_right");
            break;
        case SDLK_SPACE:
            placeBomb();
            break;
        default:
            break;
        }
    }
    else if(e.type == SDL_KEYUP)
    {
        switch(e.keysym.sym)
        {
        case SDLK_UP:
            playerComp.movementDirection.y = (playerComp.movementDirection.y <= 0.f) ? 0.f : 1.f;
            if(playerComp.movementDirection == vec2f{0,0}) animComp.playAnimation("idle_up");
            break;
        case SDLK_DOWN:
            playerComp.movementDirection.y = (playerComp.movementDirection.y >= 0.f) ? 0.f : -1.f;
            if(playerComp.movementDirection == vec2f{0,0}) animComp.playAnimation("idle_down");
            break;
        case SDLK_LEFT:
            playerComp.movementDirection.x = (playerComp.movementDirection.x <= 0.f) ? 0.f : 1.f;  
            if(playerComp.movementDirection == vec2f{0,0}) animComp.playAnimation("idle_left");
            break;
        case SDLK_RIGHT:
            playerComp.movementDirection.x = (playerComp.movementDirection.x >= 0.f) ? 0.f : -1.f;
            if(playerComp.movementDirection == vec2f{0,0}) animComp.playAnimation("idle_right");
            break;
        default:
            break;
        }
    }

    kinemComp.linearVelocity = vecNormalized(playerComp.movementDirection) * PLAYER_SPEED_MULT;
}
