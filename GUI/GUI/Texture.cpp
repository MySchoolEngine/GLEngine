#include <GUIStdafx.h>

#include <GUI/Texture.h>

#include <imgui_internal.h>

namespace GLEngine::GUI {

//=================================================================================
C_Texture::C_Texture(std::weak_ptr<Renderer::I_DeviceTexture> texture, const glm::uvec2& dim)
	: m_Texture(texture)
	, m_Dimensions(dim)
{
}

//=================================================================================
void C_Texture::Draw() const
{
	const ImVec2 iconSize(m_Dimensions.x, m_Dimensions.y);
	if (auto texture = m_Texture.lock())
	{
		ImGui::Image((void*)(intptr_t)(texture->GetDeviceTextureHandle()), iconSize);
	}
	else
	{
		const auto	canvas_pos = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list  = ImGui::GetWindowDrawList();
		auto&		style	   = ImGui::GetStyle();
		ImGui::ItemSize(iconSize);

		const ImRect total_bb(canvas_pos, canvas_pos + iconSize);
		const auto	 middle = total_bb.GetCenter().y;
		draw_list->AddRectFilled(total_bb.Min, total_bb.Max, ImGui::GetColorU32(ImGuiCol_::ImGuiCol_FrameBg));
	}
}

} // namespace GLEngine::GUI