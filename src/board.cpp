#include "board.hpp"
#include "game_params.hpp"

#include <nlohmann/json.hpp>
#include <chestnut/ecs/constants.hpp>
#include <chestnut/engine/misc/utility_functions.hpp>
#include <chestnut/engine/serialization/serializers_math.hpp>
#include <chestnut/engine/main/engine.hpp>
#include <chestnut/engine/maths/vector_cast.hpp>
#include <chestnut/engine/resources/entity_template_resource.hpp>
#include <chestnut/engine/ecs_impl/components/transform2d_component.hpp>

#include <array>
#include <cmath>
#include <fstream>


using json = nlohmann::json;

std::shared_ptr<CEntityTemplateResource> solidBlockEntTempl;
std::shared_ptr<CEntityTemplateResource> brittleBlockEntTempl;


Board::~Board() 
{
    unload();
}

void Board::unload() 
{
    auto& w = CEngine::getInstance().getEntityWorld();

    for(auto [ent, _] : m_blockMatrix)
    {
        w.destroyEntity(ent);
    }
}

void Board::load(std::filesystem::path location) 
{
    if(!solidBlockEntTempl || !brittleBlockEntTempl)
    {
        auto& r = CEngine::getInstance().getResourceManager();

        solidBlockEntTempl = r.getOrLoadResource<CEntityTemplateResource>(assetPathToAbsolute("solid_block.ent")).value();
        brittleBlockEntTempl = r.getOrLoadResource<CEntityTemplateResource>(assetPathToAbsolute("brittle_block.ent")).value();
    }


    unload();

    std::ifstream f(assetPathToAbsolute(location));

    if(f)
    {
        json j = json::parse(f);

        auto size = j.at("dimensions").get<vec2i>();
        auto vecBlocks = j.at("board").get<std::vector<int>>();

        if(size.x * size.y != vecBlocks.size())
        {
            throw std::runtime_error("Board size doesn't match");
        }

        m_dimensions = size;
        auto& w = CEngine::getInstance().getEntityWorld();

        for (int i = 0; i < size.x * size.y ; i++)
        {
            int b = vecBlocks[i];

            if(b == 1 || b == 2)
            {
                auto ent = (b == 1) 
                    ? solidBlockEntTempl->createEntity().value() 
                    : brittleBlockEntTempl->createEntity().value();

                auto blockType = (b == 1) 
                    ? BlockType::SOLID 
                    : BlockType::BRITTLE;

                m_blockMatrix.push_back({
                    ent,
                    blockType
                });
            }
            else
            {
                m_blockMatrix.push_back({
                    ENTITY_ID_INVALID,
                    BlockType::VOID
                });
            }
        }
        
        for (int y = 0; y < size.y; y++)
        {
            for (int x = 0; x < size.x; x++)
            {
                auto ent = m_blockMatrix[y * size.x + x].first;
                if(ent != ENTITY_ID_INVALID)
                {
                    w.createComponent<CTransform2DComponent>(ent)->position = {
                        (float)BLOCK_SIZE * ((float)x + 0.5f),
                        (float)BLOCK_SIZE * ((float)y + 0.5f),
                    };
                }
            }
        }
        
        
    }
}

vec2i Board::getDimensions() const
{
    return m_dimensions;
}

tl::optional<BlockType> Board::getBlockTypeAtIndex(vec2i index) const
{
    if(index.x >= 0 
    && index.x <= m_dimensions.x - 1
    && index.y >= 0
    && index.y <= m_dimensions.y - 1)
    {
        return m_blockMatrix[index.y * m_dimensions.x + index.x].second;
    }
    
    return tl::nullopt;
}

tl::optional<entityid_t> Board::getEntityAtIndex(vec2i index) const
{
    if(index.x >= 0 
    && index.x <= m_dimensions.x - 1
    && index.y >= 0
    && index.y <= m_dimensions.y - 1)
    {
        return m_blockMatrix[index.y * m_dimensions.x + index.x].first;
    }
    
    return tl::nullopt;
}

tl::optional<BlockType> Board::getBlockTypeAtPos(vec2f pos) const
{
    return getBlockTypeAtIndex(vecCastType<int>(pos / (float)BLOCK_SIZE));
}

tl::optional<entityid_t> Board::getEntityAtPos(vec2f pos) const
{
    return getEntityAtIndex(vecCastType<int>(pos / (float)BLOCK_SIZE));
}

vec2f Board::getPositionShiftedToBlockCenter(vec2f pos) const
{
    vec2f units = {
        std::floor(pos.x / (float)BLOCK_SIZE),
        std::floor(pos.y / (float)BLOCK_SIZE)
    };

    return units * (float)BLOCK_SIZE + vec2f{BLOCK_SIZE / 2.f};
}
