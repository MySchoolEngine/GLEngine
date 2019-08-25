#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/Window.h>

#include <GLRenderer/GUI/GUIPart.h>


namespace GLEngine::GLRenderer::GUI {


//=================================================================================
C_Window::C_Window(std::string&& name)
	: m_Name(std::move(name))
{

}

//=================================================================================
void C_Window::Activate(bool activate /*= true*/)
{
	m_Active = activate;
}

//=================================================================================
void C_Window::Deactivate()
{
	Activate(false);
}

//=================================================================================
bool C_Window::IsActive() const
{
	return m_Active;
}

//=================================================================================
void C_Window::Draw() const
{
	Begin();
	DrawElements();

	End();
}

//=================================================================================
void C_Window::DrawElements() const
{
	for (const auto* part : m_Elements)
	{
		part->Draw();
	}
}


//=================================================================================
void C_Window::Begin() const
{
	::ImGui::Begin(m_Name.c_str(), &m_Active);
}

//=================================================================================
void C_Window::End() const
{
	::ImGui::End();
}

//=================================================================================
void C_Window::AddGUIPart(I_GUIPart* part)
{
	m_Elements.push_back(part);
}

}