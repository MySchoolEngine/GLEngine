#include <DX12RendererStdafx.h>

#include <DX12Renderer/D3D12Window.h>
#include <DX12Renderer/D3D12WindowFactory.h>

namespace GLEngine::DX12Renderer {

//=================================================================================
std::shared_ptr<Core::I_Window> C_D3D12WindowFactory::GetWindow(const Core::S_WindowInfo& wndInfo) const
{
	if (wndInfo.m_WindowClass == "D3D12")
	{
		return std::make_shared<C_D3D12Window>(wndInfo);
	}
	return nullptr;
}

//=================================================================================
Core::I_WindowFactory* ConstructD3D12WindowFactory()
{
	return new C_D3D12WindowFactory();
}

} // namespace GLEngine::DX12Renderer