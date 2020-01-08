#include <DX12RendererStdafx.h>

#include <DX12Renderer/D3D12Window.h>

#include <DX12Renderer/D3D12WindowInfo.h>
#include <DX12Renderer/D3D12Renderer.h>

#include <winuser.h>

namespace GLEngine::DX12Renderer {

//=================================================================================
C_D3D12Window::C_D3D12Window(const Core::S_WindowInfo& wndInfo)
	: m_Window(nullptr)
	, m_HInstance(nullptr)
	, m_WantClose(false)
{
	Init(wndInfo);
}

//=================================================================================
C_D3D12Window::~C_D3D12Window()
{
	Destroy();
}

//=================================================================================
unsigned int C_D3D12Window::GetWidth() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
unsigned int C_D3D12Window::GetHeight() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
glm::uvec2 C_D3D12Window::GetSize() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
const GLEngine::Core::I_Input& C_D3D12Window::GetInput() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_D3D12Window::Update()
{
	MSG msg = {};
	// Process any messages in the queue.
	while (PeekMessage(&msg, m_Window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

//=================================================================================
void C_D3D12Window::Init(const Core::S_WindowInfo& wndInfo)
{
	const S_D3D12WindowInfo* d12wndInfo = (dynamic_cast<const S_D3D12WindowInfo*>(&wndInfo));
	m_HInstance = d12wndInfo->m_HInstance;

	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = m_HInstance;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = L"DXSampleClass";
	RegisterClassEx(&windowClass);

	RECT windowRect = { 0, 0, static_cast<LONG>(wndInfo.m_width), static_cast<LONG>(wndInfo.m_height) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	m_Window = CreateWindow(
		windowClass.lpszClassName,
		std::wstring(wndInfo.m_WindowClass.begin(), wndInfo.m_WindowClass.end()).c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,        // We have no parent window.
		nullptr,        // We aren't using menus.
		m_HInstance,
		this);

	ShowWindow(m_Window, TRUE);


	m_renderer = std::make_unique<C_D3D12Renderer>();
}

//=================================================================================
void C_D3D12Window::SetTitle(const std::string& title)
{
	SetWindowText(m_Window, std::wstring(title.begin(), title.end()).c_str());
}

//=================================================================================
bool C_D3D12Window::WantClose() const
{
	return m_WantClose;
}

//=================================================================================
const std::unique_ptr<GLEngine::Renderer::I_Renderer>& C_D3D12Window::GetRenderer() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_D3D12Window::Destroy()
{
	DestroyWindow(m_Window);
}

//=================================================================================
LRESULT CALLBACK C_D3D12Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto * window = reinterpret_cast<C_D3D12Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	switch (message)
	{
		case WM_CREATE:
		{
			auto pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
			return 0;
		}
		case WM_CLOSE:
		{
			window->m_WantClose = true;
			return 0;
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

}
