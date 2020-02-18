#pragma once

#include <DX12Renderer/DX12RendererApi.h>

#include <Core/IWindowManager.h>


namespace GLEngine::DX12Renderer {

class C_D3D12WindowFactory final : public Core::I_WindowFactory{
public:
	C_D3D12WindowFactory() = default;
	virtual std::shared_ptr<Core::I_Window> GetWindow(const Core::S_WindowInfo&) const override;
};

DX12_RENDERER_API_EXPORT Core::I_WindowFactory* ConstructD3D12WindowFactory();
}