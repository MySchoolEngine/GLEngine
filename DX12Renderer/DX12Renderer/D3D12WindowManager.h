#pragma once

#include <Core/CoreMacros.h>

#include <Core/IWindowManager.h>

#include <vector>

namespace GLEngine::DX12Renderer {

class C_D3D12WindowManager : public Core::I_WindowManager {
public:
	C_D3D12WindowManager(Core::C_Application::EventCallbackFn eventCallback, HINSTANCE hInstance);
	//=============================================================
	// I_WindowManager
	//=============================================================
	virtual std::shared_ptr<Core::I_Window> OpenNewWindow(const Core::S_WindowInfo& info) override;
	virtual std::shared_ptr<Core::I_Window> GetWindow(GUID guid) const override;
	virtual void Update() override;
	virtual unsigned int NumWindows() const override;


	//=================================================================================
	// Core::C_Layer
	//=================================================================================
	virtual void OnEvent(Core::I_Event& event) override;

protected:
	void Init();
	HINSTANCE m_hInstance;

private:
	std::vector<std::shared_ptr<Core::I_Window>> m_Windows;
};

API_EXPORT Core::I_WindowManager* ConstructD3D12Manager(Core::C_Application::EventCallbackFn eventCallback, HINSTANCE hInstance);
}