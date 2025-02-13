#pragma once

namespace GLEngine::Renderer
{
struct IndirectDraw
{
public:
    std::uint32_t    vertexCount;
    std::uint32_t    instanceCount;
    std::uint32_t    firstVertex;
    std::uint32_t    firstInstance;
};

struct IndirectDrawIndexed
{
public:
    std::uint32_t    indexCount;
    std::uint32_t    instanceCount;
    std::uint32_t    firstIndex;
    std::int32_t     vertexOffset;
    std::uint32_t    firstInstance;
};
}