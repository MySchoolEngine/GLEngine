#include <GUIStdafx.h>

#include <GUI/Image.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

#include <imgui_internal.h>

namespace GLEngine::GUI {

//=================================================================================
C_Image::C_Image(Renderer::Handle<Renderer::Texture> handle)
	: m_texture(handle)
	, m_Zoom(2.f)
{
}

//=================================================================================
void C_Image::Draw() const
{
	if (m_texture.IsValid())
	{
		void* gpuHandle = Core::C_Application::Get().GetActiveRenderer().GetTextureGPUHandle(m_texture);
		ImGui::Image((void*)(intptr_t)(gpuHandle), ImVec2(m_Size.x, m_Size.y));
	}
}

//=================================================================================
void C_Image::SetSize(const glm::vec2 dim)
{
	m_Size = dim;
}

} // namespace GLEngine::GUI