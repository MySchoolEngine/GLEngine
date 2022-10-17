#pragma once

#include <Renderer/Textures/TextureDefinitions.h>

namespace GLEngine::VkRenderer {

//=================================================================================
VkFormat GetVkInternalFormat(const Renderer::E_TextureFormat format);
} // namespace GLEngine::VkRenderer
