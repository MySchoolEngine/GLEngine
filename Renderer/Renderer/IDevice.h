#pragma once

#include <Renderer/Descriptors/TextureDescriptor.h>
#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {
class I_Device {
public:
	I_Device()			= default;
	virtual ~I_Device() = default;

	virtual void CreateTexture(TextureDescriptor& descriptor, void** texture) = 0;
};
} // namespace GLEngine::Renderer
