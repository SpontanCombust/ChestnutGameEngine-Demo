#pragma warning(disable : 4251 4834 4005)
#define CHESTNUT_DEBUG

#include "components/block_component.hpp"
#include "components/bomb_component.hpp"
#include "components/explosion_component.hpp"
#include "components/player_component.hpp"
#include "components/board_component.hpp"
#include "systems/bomb_system.hpp"
#include "systems/explosion_system.hpp"
#include "systems/player_system.hpp"
#include "board.hpp"


#include <chestnut/engine/init.hpp>
#include <chestnut/engine/debug/log.hpp>
#include <chestnut/engine/debug/component_rtti.hpp>
#include <chestnut/engine/main/engine.hpp>
#include <chestnut/engine/ecs_impl/systems/input_event_dispatch_system.hpp>
#include <chestnut/engine/ecs_impl/systems/simple2d_rendering_system.hpp>
#include <chestnut/engine/ecs_impl/systems/simple2d_animation_system.hpp>
#include <chestnut/engine/ecs_impl/systems/simple2d_collision_system.hpp>
#include <chestnut/engine/ecs_impl/systems/kinematics2d_system.hpp>
#include <chestnut/engine/ecs_impl/systems/debug_gui_rendering_system.hpp>
#include <chestnut/engine/ecs_impl/components/identity_component.hpp>
#include <chestnut/engine/serialization/serializers_ecs_impl/serializers_components.hpp>
#include <chestnut/engine/resources/entity_template_resource.hpp>
#include <chestnut/engine/misc/utility_functions.hpp>
#include <chestnut/engine/maths/matrix4.hpp>
#include <chestnut/engine/maths/vector_transform.hpp>

#include <windows.h>


using namespace chestnut::engine;

class CMainControllerSystem : public ILogicSystem
{
public:
    void onAttach() override
    {
        auto &w = CEngine::getInstance().getEntityWorld();

        entityid_t boardEnt = w.createEntity();
        w.createComponent<BoardComponent>(boardEnt)->board.load("level1.board");
        strcpy_s(w.createComponent<CIdentityComponent>(boardEnt)->name, "Board");

        CEngine::getInstance()
        .getResourceManager()
        .getOrLoadResource<CEntityTemplateResource>(assetPathToAbsolute("player.ent"))
        .value()->createEntity().value();
    }

    void update(float dt) override
    {

    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{   
    try
    {
        debug::CLogger::setToFile("bomberman.log");

        chestnutInit();

        CWindow window(
            CWindowAttribs("Bomberman")
            .width(1280)
            .height(720)
            .position({100, 100})
            .vsync(true)
        );

        CEngine::createInstance(window, 1.f/60.f);
        auto& engine = CEngine::getInstance();

        debug::registerComponentRTTI<BlockComponent>();
        debug::registerComponentRTTI<BombComponent>();
        debug::registerComponentRTTI<ExplosionComponent>();
        debug::registerComponentRTTI<PlayerComponent>();

        engine.attachSystem(new CInputEventDispatchSystem);
        engine.attachSystem(new CMainControllerSystem);
        engine.attachSystem(new PlayerSystem);
        engine.attachSystem(new BombSystem);
        engine.attachSystem(new ExplosionSystem);
        engine.attachSystem(new CKinematics2DSystem);
        engine.attachSystem(new CSimple2DCollisionSystem);
        engine.attachSystem(new CSimple2DAnimationSystem);
        engine.attachSystem(new CSimple2DRenderingSystem);
        engine.attachSystem(new CDebugGuiRenderingSystem);

        engine.start();

        chestnutQuit();
    }
    catch(const std::exception& e)
    {
        LOG_ERROR(e.what());
        debug::CLogger::flush();
        return -1;
    }

    return 0;
}
