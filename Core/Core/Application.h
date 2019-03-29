#pragma once

#include <Core/CoreMacros.h>

namespace GLEngine {
namespace Core {

class I_WindowManager;

class API_EXPORT C_Application {
public:
	C_Application();
	virtual ~C_Application() = default;

	void LoadArgs(int argc, char** argv);

	void Run();

	virtual void Init() = 0;

	inline static C_Application& Get() { return *s_Instance; }
protected:
	virtual Core::I_WindowManager& GetWndMgr() const = 0;
private:
	static C_Application* s_Instance;
};

// To be defined in CLIENT
C_Application* CreateApplication();

}}