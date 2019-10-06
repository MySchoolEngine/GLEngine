#pragma once

#include <Core/CoreMacros.h>

#include <Core/IWindowManager.h>


namespace GLEngine::DX12Renderer {

class C_D3D12WindowFactory final : public Core::I_WindowFactory{
public:
	C_D3D12WindowFactory() = default;
	virtual std::shared_ptr<Core::I_Window> GetWindow(const Core::S_WindowInfo&) const override;
};

API_EXPORT Core::I_WindowFactory* ConstructD3D12WindowFactory();
}