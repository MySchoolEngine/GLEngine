#pragma once

#include <Core/Application.h>
#include <Core/IWindow.h>

#include <Windows.h>

namespace GLEngine::DX12Renderer {

class C_D3D12Window : public Core::I_Window {
public:
	explicit C_D3D12Window(const Core::S_WindowInfo& wndInfo);
	~C_D3D12Window();
	//============================================================
	// I_Window
	//============================================================
	[[nodiscard]] virtual unsigned int			GetWidth() const override;
	[[nodiscard]] virtual unsigned int			GetHeight() const override;
	[[nodiscard]] virtual glm::uvec2			GetSize() const override;
	[[nodiscard]] virtual const Core::I_Input&	GetInput() const override;
	virtual void								Update() override;
	virtual void								Init(const Core::S_WindowInfo& wndInfo) override;
	virtual void								SetTitle(const std::string& title) override;
	[[nodiscard]] virtual bool					WantClose() const override;
	[[nodiscard]] virtual Renderer::I_Renderer& GetRenderer() override;

	[[nodiscard]] virtual glm::vec2 ToClipSpace(const glm::vec2& screenCoord) const override;

protected:
	virtual void Destroy() override;

	HWND	  m_Window;
	HINSTANCE m_HInstance;
	bool	  m_WantClose;

private:
	static LRESULT CALLBACK				  WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	std::unique_ptr<Renderer::I_Renderer> m_renderer;
};
} // namespace GLEngine::DX12Renderer