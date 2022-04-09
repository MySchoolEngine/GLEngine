#include <DX12RendererStdAfx.h>

#include <DX12Renderer/D3D12Device.h>

namespace GLEngine::DX12Renderer {

//=================================================================================
std::size_t C_D3D12Device::GetAllocatedMemory() const
{
	return 0;
}

//=================================================================================
void C_D3D12Device::DestroyTexture(Renderer::I_DeviceTexture& texture)
{
}

//=================================================================================
bool C_D3D12Device::AllocateTexture(Renderer::I_DeviceTexture& texture)
{
	return false;
}

//=================================================================================
bool C_D3D12Device::AllocateSampler(Renderer::I_TextureSampler2D& texture)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_D3D12Device::DestroySampler(Renderer::I_TextureSampler2D& texture)
{
	throw std::logic_error("The method or operation is not implemented.");
}

} // namespace GLEngine::DX12Renderer
