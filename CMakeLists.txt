cmake_minimum_required(VERSION 3.0.0)
project(Bomberman VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)


# =============================== ALLOW OFFLINE MODE ===================================

FetchContent_Declare(
    FetchContentOffline
    GIT_REPOSITORY https://github.com/SpontanCombust/cmake-fetchcontent-offline
)
set(FETCHCONTENT_UPDATES_DISCONNECTED_FETCHCONTENTOFFLINE ON)
FetchContent_MakeAvailable(FetchContentOffline)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${fetchcontentoffline_SOURCE_DIR}")
include(fetchcontent-offline)

FetchContent_DisconnectedIfOffline()

# =====================================================================================


FetchContent_Declare(
    ChestnutGameEngine
    GIT_REPOSITORY https://github.com/SpontanCombust/ChestnutGameEngine
    GIT_TAG af845714a8fa8f262c6dda8ddbf443b1d5e4772d # dev 03/02/2023
)

set(CHESTNUT_BUILD_ENGINE_EDITOR OFF)
FetchContent_MakeAvailable(ChestnutGameEngine)


add_executable(Bomberman WIN32 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/components/board_component.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/components/board_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/components/block_component.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/components/bomb_component.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/components/explosion_component.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/components/explosion_emitter_component.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/components/player_component.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/events/explosion_event.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/systems/bomb_system.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/systems/bomb_system.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/systems/explosion_system.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/systems/explosion_system.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/systems/player_system.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/systems/player_system.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/board.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/board.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

target_link_libraries(Bomberman PRIVATE ChestnutGameEngine)
target_include_directories(Bomberman PRIVATE src)
target_compile_options(Bomberman PRIVATE /wd4100 /wd4101 /wd4834 /wd4251)

copy_shared_libs(TARGET Bomberman DEPENDENCIES
    SDL2 
    SDL2_ttf
    SDL2_mixer
    freetype
    ChestnutGameEngine
)