#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/GUIWindow.h>

#include <Core/GUID.h>

namespace GLEngine::GLRenderer::GUI {

//=================================================================================
C_Window::C_Window(GUID guid, const std::string& name) : m_GUID(guid)
, m_Name(name)
, m_IsVisible(false)
{

}

//=================================================================================
void C_Window::Draw() const
{
	::ImGui::Begin(m_Name.c_str(), &m_IsVisible);
	for (const auto& component : m_Components)
	{
		component.second.get().Draw();
	}
	::ImGui::End();
}

//=================================================================================
void C_Window::SetVisible(bool enable /*= true*/)
{
	m_IsVisible = enable;
}

//=================================================================================
bool C_Window::IsVisible() const
{
	return m_IsVisible;
}

//=================================================================================
GUID C_Window::AddComponent(C_Window::T_GUIPartRef component)
{
	GUID guid = NextGUID();
	m_Components.emplace(guid, component);
	return guid;
}

//=================================================================================
I_GUIPart* C_Window::GetComponent(GUID guid) const
{
	auto& it = m_Components.find(guid);
	if (it != m_Components.end())
	{
		return &(it->second.get());
	}
	return nullptr;
}

}