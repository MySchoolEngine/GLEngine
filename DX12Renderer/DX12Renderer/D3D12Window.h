#pragma once

#include <Core/IWindow.h>
#include <Core/Application.h>

#include <Windows.h>

namespace GLEngine::DX12Renderer {

class API_EXPORT C_D3D12Window : public Core::I_Window {
public:
	C_D3D12Window(const Core::S_WindowInfo& wndInfo);
	~C_D3D12Window();
	//============================================================
	// I_Window
	//============================================================
	virtual unsigned int GetWidth() const override;
	virtual unsigned int GetHeight() const override;
	virtual glm::uvec2 GetSize() const override;
	virtual const Core::I_Input& GetInput() const override;
	virtual void Update() override;
	virtual void Init(const Core::S_WindowInfo& wndInfo) override;
	virtual void SetTitle(const std::string& title) override;
	virtual bool WantClose() const override;
	virtual const std::unique_ptr<GLEngine::Renderer::I_Renderer>& GetRenderer() const override;

protected:
	virtual void Destroy() override;

	HWND                    m_Window;
	HINSTANCE				m_HInstance;
	bool					m_WantClose;
private:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	std::unique_ptr<Renderer::I_Renderer> m_renderer;
};
}