#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Descriptors/TextureDescriptor.h>

namespace GLEngine::Renderer {
class I_Device {
public:
	I_Device() = default;
	virtual ~I_Device() = default;

	virtual void CreateTexture(TextureDescriptor& descriptor, void** texture) = 0;
};
}

