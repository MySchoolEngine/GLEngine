#include <DX12RendererStdAfx.h>

#include <DX12Renderer/D3D12Device.h>

namespace GLEngine::DX12Renderer {

//=================================================================================
std::size_t C_D3D12Device::GetAllocatedMemory() const
{
	return 0;
}

//=================================================================================
void C_D3D12Device::DestroyTexture(T_TextureHandle& texture)
{
}

//=================================================================================
bool C_D3D12Device::AllocateTexture(Renderer::I_DeviceTexture& texture)
{
	return false;
}

} // namespace GLEngine::DX12Renderer
