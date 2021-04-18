#pragma once

#include <DX12Renderer/DX12RendererApi.h>

#include <Core/IWindowManager.h>

#include <vector>

namespace GLEngine::DX12Renderer {

class C_D3D12WindowManager : public Core::I_WindowManager {
public:
	C_D3D12WindowManager(Core::C_Application::EventCallbackFn eventCallback, HINSTANCE hInstance);
	//=============================================================
	// I_WindowManager
	//=============================================================
	virtual std::shared_ptr<Core::I_Window>		OpenNewWindow(const Core::S_WindowInfo& info) override;
	virtual std::shared_ptr<Core::I_Window>		GetWindow(GUID guid) const override;
	virtual void								Update() override;
	virtual unsigned int						NumWindows() const override;
	virtual Renderer::I_Renderer&				GetActiveRenderer() override;
	[[nodiscard]] virtual Renderer::I_Renderer* ActiveRendererPtr() override;

	//=================================================================================
	// Core::C_Layer
	//=================================================================================
	virtual void OnEvent(Core::I_Event& event) override;

protected:
	void	  Init();
	HINSTANCE m_hInstance;

private:
	std::vector<std::shared_ptr<Core::I_Window>> m_Windows;
	std::shared_ptr<Core::I_Window>				 m_UpdatingWindow;
};

DX12_RENDERER_API_EXPORT C_D3D12WindowManager* ConstructD3D12Manager(Core::C_Application::EventCallbackFn eventCallback, HINSTANCE hInstance);
} // namespace GLEngine::DX12Renderer