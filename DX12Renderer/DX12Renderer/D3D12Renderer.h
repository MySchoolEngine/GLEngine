#pragma once

#include <Renderer/IRenderer.h>

namespace GLEngine::DX12Renderer {

class C_D3D12Renderer : public Renderer::I_Renderer {
public:
	C_D3D12Renderer();
	virtual ~C_D3D12Renderer() override;

	//=================================================
	// Renderer::I_Renderer
	//=================================================
	void Lock(bool lock = true) override;
	void AddCommand(T_CommandPtr) override;
	void AddTransferCommand(T_CommandPtr) override;
	void Commit() const override;
	void ClearCommandBuffers() override;
	void SetBufferData(Renderer::Handle<Renderer::Buffer> dstBuffer, std::size_t numBytes, const void* data) override;
	void SetTextureSampler(Renderer::Handle<Renderer::Texture> dstTexture, Renderer::Handle<Renderer::Sampler> srcSampler) override;

	//=================================================================================
	Renderer::E_PassType	  GetCurrentPassType() const override;
	Renderer::I_Device&		  GetDevice() override;
	Renderer::ResourceManager& GetRM() override;



private:
	bool											 m_Locked = false;
	std::vector<Renderer::I_Renderer::T_CommandPtr>* m_CommandQueue;
};
} // namespace GLEngine::DX12Renderer