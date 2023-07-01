#pragma once

#include <GLRenderer/GLResourceManager.h>

#include <Renderer/IDevice.h>

namespace GLEngine::GLRenderer {
class C_Framebuffer;
class C_GLDevice : public Renderer::I_Device {
public:
	C_GLDevice();
	[[nodiscard]] virtual bool AllocateTexture(Renderer::I_DeviceTexture& texture) override;
	virtual void			   DestroyTexture(Renderer::I_DeviceTexture& texture) override;
	virtual T_TextureHandle	   CreateTextureHandle(const Renderer::TextureDescriptor& desc) override;


	C_Framebuffer* AllocateFramebuffer(const std::string_view name);
	C_Framebuffer  GetDefualtRendertarget();

	virtual std::size_t GetAllocatedMemory() const override;


	[[nodiscard]] virtual bool AllocateSampler(Renderer::I_TextureSampler2D& texture) override;
	virtual void			   DestroySampler(Renderer::I_TextureSampler2D& texture) override;

	GLResourceManager& GetRM();
private:
	[[nodiscard]] bool HasExtension(const std::string_view ext) const;

	std::size_t m_TotalMemory; //< Based on available memory at start time
	std::size_t m_MemoryUsed;  //< my very rough estimate!!

	std::size_t m_TextureBudget;
	std::size_t m_TextureMemoryUsed;


	GLResourceManager m_GPUResourceManager;
};
} // namespace GLEngine::GLRenderer
