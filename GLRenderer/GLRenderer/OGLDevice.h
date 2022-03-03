#pragma once

#include <Renderer/IDevice.h>

namespace GLEngine::GLRenderer {

class C_GLDevice : public Renderer::I_Device {
public:
	C_GLDevice();
	virtual [[nodiscard]] bool AllocateTexture(Renderer::I_DeviceTexture& texture) override;
	virtual void			   DestroyTexture(T_TextureHandle& texture) override;

	virtual std::size_t GetAllocatedMemory() const override;


private:
	[[nodiscard]] bool HasExtension(const std::string_view ext) const;

	std::size_t m_TotalMemory; //< Based on available memory at start time
	std::size_t m_MemoryUsed;  //< my very rough estimate!!
};
} // namespace GLEngine::GLRenderer