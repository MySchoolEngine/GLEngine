#include <GUIStdafx.h>

#include <GUI/Image.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

namespace GLEngine::GUI {

//=================================================================================
C_Image::C_Image(const Renderer::Handle<Renderer::Texture> handle)
	: m_Texture(handle)
	, m_Zoom(2.f)
	, m_GUIHandle(nullptr)
{
	if (m_Texture.IsValid()) {
		m_GUIHandle = Core::C_Application::Get().GetActiveRenderer().GetTextureGUIHandle(m_Texture);
	}
}

//=================================================================================
bool C_Image::Draw() const
{
	if (m_Texture.IsValid() && m_GUIHandle)
	{
		ImGui::Image((void*)(intptr_t)(m_GUIHandle), ImVec2(m_Size.x, m_Size.y));
	}
	return false;
}

//=================================================================================
void C_Image::SetSize(const glm::vec2 dim)
{
	m_Size = dim;
}

} // namespace GLEngine::GUI