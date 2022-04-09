#pragma once

#include <Renderer/IDevice.h>

namespace GLEngine::DX12Renderer {
class C_D3D12Device : public Renderer::I_Device {
public:
	[[nodiscard]] bool		  AllocateTexture(Renderer::I_DeviceTexture& texture) override;
	void					  DestroyTexture(Renderer::I_DeviceTexture& texture) override;
	[[nodiscard]] std::size_t GetAllocatedMemory() const override;

	bool AllocateSampler(Renderer::I_TextureSampler2D& texture) override;
	void DestroySampler(Renderer::I_TextureSampler2D& texture) override;
};
} // namespace GLEngine::DX12Renderer
