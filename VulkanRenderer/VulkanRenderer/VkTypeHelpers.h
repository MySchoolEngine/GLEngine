#pragma once

#include <Renderer/Definitions.h>
#include <Renderer/Textures/TextureDefinitions.h>

namespace GLEngine::Renderer {
class C_Viewport;
}

namespace GLEngine::VkRenderer {

VkFormat			  GetVkInternalFormat(const Renderer::E_TextureFormat format);
VkShaderStageFlagBits GetVkShaderStage(Renderer::E_ShaderStage stage);
VkViewport			  TranslateViewport(const Renderer::C_Viewport& viewport);
} // namespace GLEngine::VkRenderer
