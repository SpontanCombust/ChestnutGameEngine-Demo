#pragma once

#include <chestnut/ecs/types.hpp>
#include <chestnut/engine/maths/vector2.hpp>
#include <tl/optional.hpp>

#include <filesystem>
#include <utility>
#include <vector>


using namespace chestnut::ecs;
using namespace chestnut::engine;

enum class BlockType
{
    VOID,
    SOLID,
    BRITTLE    
};

class Board
{
private:
    using BlockElement = std::pair<entityid_t, BlockType>;
    
    vec2i m_dimensions;
    std::vector<BlockElement> m_blockMatrix;

public:
    ~Board();

    void load(std::filesystem::path location);
    void unload();

    vec2i getDimensions() const;

    // Returns nullopt if index is out of board bounds
    tl::optional<BlockType> getBlockTypeAtIndex(vec2i index) const;
    // Returns nullopt if index is out of board bounds
    // Returns ENTITY_ID_INVALID if block type is VOID
    tl::optional<entityid_t> getEntityAtIndex(vec2i index) const;

    // Returns nullopt if index is out of board bounds  
    tl::optional<BlockType> getBlockTypeAtPos(vec2f pos) const;
    // Returns nullopt if index is out of board bounds
    // Returns nullopt if index is out of board bounds
    tl::optional<entityid_t> getEntityAtPos(vec2f pos) const;

    vec2f getPositionShiftedToBlockCenter(vec2f pos) const;
};