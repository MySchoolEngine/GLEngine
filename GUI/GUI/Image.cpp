#include <GUIStdafx.h>

#include <GUI/Image.h>

#include <imgui_internal.h>

namespace GLEngine::GUI {

//=================================================================================
C_Image::C_Image(Renderer::I_DeviceTexture& texture)
	: m_texture(texture)
	, m_Zoom(2.f)
{
	SetSize(m_texture.get().GetDimensions());
}

//=================================================================================
void C_Image::Draw() const
{
	ImGui::Image((void*)(intptr_t)(m_texture.get().GetGPUHandle()), ImVec2(m_Size.x, m_Size.y));
}

//=================================================================================
void C_Image::SetSize(const glm::vec2 dim)
{
	m_Size = dim;
}

} // namespace GLEngine::GUI