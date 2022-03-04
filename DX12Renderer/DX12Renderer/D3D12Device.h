#pragma once

#include <Renderer/IDevice.h>

namespace GLEngine::DX12Renderer {
class C_D3D12Device : public Renderer::I_Device {
public:
	[[nodiscard]] bool		  AllocateTexture(Renderer::I_DeviceTexture& texture) override;
	void					  DestroyTexture(T_TextureHandle& texture) override;
	[[nodiscard]] std::size_t GetAllocatedMemory() const override;
};
} // namespace GLEngine::DX12Renderer
