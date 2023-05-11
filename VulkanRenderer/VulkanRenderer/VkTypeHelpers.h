#pragma once

#include <Renderer/Definitions.h>
#include <Renderer/Textures/TextureDefinitions.h>
#include <Renderer/Descriptors/BufferDescriptor.h>

namespace GLEngine::Renderer {
class C_Viewport;
}

namespace GLEngine::VkRenderer {

VkFormat			  GetVkInternalFormat(const Renderer::E_TextureFormat format);
VkFilter			  GetVkInternalFormat(const Renderer::E_TextureFilter format);
VkSamplerAddressMode  GetVkInternalFormat(const Renderer::E_WrapFunction wrapFunction);
VkFormat			  GetVkShaderDataFormat(const Renderer::E_ShaderDataType format);
VkShaderStageFlagBits GetVkShaderStage(Renderer::E_ShaderStage stage);
VkViewport			  TranslateViewport(const Renderer::C_Viewport& viewport);
VkPrimitiveTopology	  GetVkDrawPrimitive(const Renderer::E_RenderPrimitives renderPrimitive);
VkBufferUsageFlags	  GetBufferType(const Renderer::E_BufferType bufferType);
} // namespace GLEngine::VkRenderer
