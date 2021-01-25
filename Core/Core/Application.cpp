#include <CoreStdafx.h>

#include <Core/Application.h>
#include <Core/IWindowManager.h>

namespace GLEngine { namespace Core {

C_Application* s_Instance = nullptr;

//=================================================================================
C_Application::C_Application()
{
	s_Instance = this;
}

//=================================================================================
void C_Application::LoadArgs(int argc, char** argv)
{
}

//=================================================================================
void C_Application::Run()
{
	auto& wmng = GetWndMgr();

	while (wmng.NumWindows())
	{
		wmng.Update();
	}
}

//=================================================================================
void C_Application::OnEvent(I_Event& e)
{
	auto& wmng = GetWndMgr();

	wmng.OnEvent(e);
}

//=================================================================================
GLEngine::Core::C_Application& C_Application::Get()
{
	return *s_Instance;
}

}} // namespace GLEngine::Core