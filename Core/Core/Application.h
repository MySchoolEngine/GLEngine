#pragma once

namespace GLEngine {

namespace Renderer {
class I_Renderer;
}

namespace Core {

class I_Event;
class I_WindowManager;

class API_EXPORT C_Application {
public:
	using EventCallbackFn = std::function<void(I_Event&)>;

	C_Application();
	virtual ~C_Application() = default;

	void LoadArgs(int argc, char** argv);

	void Run();

	void OnEvent(I_Event& e);

	virtual void Init() = 0;

	virtual const std::unique_ptr<GLEngine::Renderer::I_Renderer>& GetActiveRenderer() const = 0;

	static C_Application& Get();
	virtual Core::I_WindowManager& GetWndMgr() const = 0;
};

// To be defined in CLIENT
C_Application* CreateApplication();

}}