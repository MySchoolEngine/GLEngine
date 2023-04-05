#include <GUIStdafx.h>

#include <GUI/ImageViewer.h>

#include <imgui_internal.h>

namespace GLEngine::GUI {

//=================================================================================
C_ImageViewer::C_ImageViewer(Renderer::I_DeviceTexture& texture)
	: m_texture(texture)
	, m_Zoom(2.f)
{
	SetSize(m_texture.get().GetDimensions());
}

//=================================================================================
void C_ImageViewer::Draw() const
{
	// drawing area
	const auto io = ImGui::GetIO();
	m_Zoom		  = std::clamp(m_Zoom + io.MouseWheel / 10.f, 1.f, 10.f);

	const ImVec2 drawAreaSz(m_Size.x, m_Size.y);
	const ImVec2 canvas_p0 = ImGui::GetCursorPos();
	const ImRect imageRect(canvas_p0, canvas_p0 + drawAreaSz);
	ImGui::InvisibleButton("canvas", drawAreaSz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
	const bool is_hovered = ImGui::IsItemHovered(); // Hovered
	const bool is_active  = ImGui::IsItemActive();	// Held
	ImGui::SetCursorPos(canvas_p0);
	ImRect zoomArea({0, 0}, {1, 1});
	if (is_hovered)
	{
		// mouse pos is defined in screen space
		const auto mousePos = ImGui::GetMousePos() - ImGui::GetCursorScreenPos();
		ImVec2 mouseUV(mousePos.x / drawAreaSz.x, mousePos.y / drawAreaSz.y);
		mouseUV.x	 = glm::clamp(mouseUV.x, 1.f / m_Zoom / 2.f, 1.f - 1.f / m_Zoom / 2.f);
		mouseUV.y	 = glm::clamp(mouseUV.y, 1.f / m_Zoom / 2.f, 1.f - 1.f / m_Zoom / 2.f);
		zoomArea.Min = mouseUV - ImVec2{1.f / m_Zoom / 2.f, 1.f / m_Zoom / 2.f};
		zoomArea.Max = mouseUV + ImVec2{1.f / m_Zoom / 2.f, 1.f / m_Zoom / 2.f};
	}

	// todo - minimap - done
	//      - channels

	ImGui::Image((void*)(intptr_t)(m_texture.get().GetGPUHandle()), drawAreaSz, zoomArea.Min, zoomArea.Max);

	const ImVec2 canvas_final = ImGui::GetCursorPos();

	// === minimap ===
	// show only when zoomed and mouse over
	if (is_hovered && m_Zoom != 1.f)
	{
		constexpr float offsetFromCorner = 10.f;
		constexpr float maxWidth		 = 250.f;
		const float		width			 = std::min(drawAreaSz.x / 5.f, maxWidth);
		const float		height			 = width * (m_Size.y / m_Size.x);
		const ImVec2	minimapDrawAreaSz(width, height);
		ImGui::SetCursorPos(canvas_p0 + ImVec2(drawAreaSz.x - offsetFromCorner - minimapDrawAreaSz.x, offsetFromCorner));
		const ImVec2 screenSpacePos = ImGui::GetCursorScreenPos();
		ImGui::Image((void*)(intptr_t)(m_texture.get().GetGPUHandle()), minimapDrawAreaSz);

		// rect
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		ImVec2		 minPos(width*zoomArea.Min.x, height * zoomArea.Min.y);
		ImVec2		 maxPos(width*zoomArea.Max.x, height * zoomArea.Max.y);

		window->DrawList->AddRect(screenSpacePos + minPos, screenSpacePos + maxPos, ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 0.f, 0.f, .5f)), 0.0f);
	}
	ImGui::SetCursorPos(canvas_p0);
	ImGui::ItemSize(imageRect);
}

//=================================================================================
void C_ImageViewer::SetSize(const glm::vec2 dim)
{
	m_Size = dim;
}

} // namespace GLEngine::GUI