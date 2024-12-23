#include <GUIStdafx.h>

#include <GUI/GUIUtils.h>
#include <GUI/Texture.h>

#include <imgui_internal.h>

namespace GLEngine::GUI {

//=================================================================================
C_Texture::C_Texture(const std::weak_ptr<Renderer::I_DeviceTexture>& texture, const glm::uvec2& dim)
	: m_Texture(texture)
	, m_Dimensions(dim)
{
}

//=================================================================================
bool C_Texture::Draw() const
{
	bool		 changed = false;
	const ImVec2 iconSize(static_cast<float>(m_Dimensions.x), static_cast<float>(m_Dimensions.y));
	const auto	 canvasPos = ImGui::GetCursorScreenPos();
	ImDrawList*	 drawList  = ImGui::GetWindowDrawList();
	const ImRect totalBb(canvasPos, canvasPos + iconSize);
	if (auto texture = m_Texture.lock())
	{
		ImGui::Image((void*)(intptr_t)(texture->GetDeviceTextureHandle()), iconSize);

		const ImGuiIO& io = ImGui::GetIO();
		if (totalBb.Contains(io.MousePos))
		{
			if (DrawSquareButton(drawList, canvasPos + ImVec2(iconSize.x, 0) - ImVec2(20, -4), E_ButtonType::Cross) && io.MouseReleased[0] && m_TextureCleanCB)
			{
				m_TextureCleanCB();
				changed = true;
			}
		}
	}
	else
	{
		auto& style = ImGui::GetStyle();
		ImGui::ItemSize(iconSize);

		const auto middle = totalBb.GetCenter().y;
		drawList->AddRectFilled(totalBb.Min, totalBb.Max, ImGui::GetColorU32(ImGuiCol_::ImGuiCol_FrameBg));
		const auto cursorPosBak = ImGui::GetCursorPos();
		ImGui::SetCursorPos(cursorPosBak - ImVec2(0, iconSize.y / 2.0f));
		if (ImGui::Button("Set image"))
		{
			changed = true;
			// missing part
		}
		ImGui::SetCursorPos(cursorPosBak);
	}
	return changed;
}

//=================================================================================
void C_Texture::SetOnTextureCleanCB(const std::function<void()>& cb)
{
	m_TextureCleanCB = cb;
}

} // namespace GLEngine::GUI