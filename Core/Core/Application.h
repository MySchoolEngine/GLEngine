#pragma once

#include <Core/CoreApi.h>

namespace GLEngine {

namespace Renderer {
class I_Renderer;
}

namespace Core {

class I_Event;
class I_WindowManager;

class CORE_API_EXPORT C_Application {
public:
	using EventCallbackFn = std::function<void(I_Event&)>;

	C_Application();
	virtual ~C_Application() = default;

	void LoadArgs(int argc, char** argv);

	void Run();

	void OnEvent(I_Event& e);

	virtual void Init() = 0;
#if CORE_PLATFORM == CORE_PLATFORM_WIN
	virtual void Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow){}
#endif

	virtual Renderer::I_Renderer&						  GetActiveRenderer()	 = 0;
	[[nodiscard]] virtual Renderer::I_Renderer* GetActiveRendererPtr() = 0;

	static C_Application&		   Get();
	virtual Core::I_WindowManager& GetWndMgr() const = 0;
};

// To be defined in CLIENT
C_Application* CreateApplication();

} // namespace Core
} // namespace GLEngine