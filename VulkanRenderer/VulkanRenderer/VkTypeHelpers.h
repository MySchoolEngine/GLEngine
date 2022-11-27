#pragma once

#include <Renderer/Textures/TextureDefinitions.h>
#include <Renderer/Buffer.h>

namespace GLEngine::VkRenderer {

//=================================================================================
VkFormat GetVkInternalFormat(const Renderer::E_TextureFormat format);

VkBufferUsageFlagBits GetVkBufferUsage(const Renderer::E_BufferType);
} // namespace GLEngine::VkRenderer
