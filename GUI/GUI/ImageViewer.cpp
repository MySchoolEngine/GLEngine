#include <GUIStdafx.h>

#include <GUI/ImageViewer.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace GLEngine::GUI {

//=================================================================================
C_ImageViewer::C_ImageViewer(const Renderer::Handle<Renderer::Texture> texture)
	: m_Texture(texture)
	, m_Zoom(2.f)
	, m_GUIHandle(nullptr)
{
}

//=================================================================================
bool C_ImageViewer::Draw() const
{
	// TODO
	if (m_Texture.IsValid() == false)
		return false;
	// drawing area

	const ImVec2 drawAreaSz(m_Size.x, m_Size.y);
	const ImVec2 canvasP0 = ImGui::GetCursorPos();
	const ImRect imageRect(canvasP0, canvasP0 + drawAreaSz);
	ImGui::InvisibleButton("canvas", drawAreaSz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
	const bool isHovered = ImGui::IsItemHovered(); // Hovered
	const bool isActive	 = ImGui::IsItemActive();  // Held
	ImGui::SetCursorPos(canvasP0);
	ImRect zoomArea({0, 0}, {1, 1});
	if (isHovered)
	{
		const auto io = ImGui::GetIO();
		m_Zoom = std::clamp(m_Zoom + io.MouseWheel / 10.f, 1.f, 10.f);
		::ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);
		// mouse pos is defined in screen space
		const auto mousePos = ImGui::GetMousePos() - ImGui::GetCursorScreenPos();
		ImVec2	   mouseUV(mousePos.x / drawAreaSz.x, mousePos.y / drawAreaSz.y);
		mouseUV.x	 = glm::clamp(mouseUV.x, 1.f / m_Zoom / 2.f, 1.f - 1.f / m_Zoom / 2.f);
		mouseUV.y	 = glm::clamp(mouseUV.y, 1.f / m_Zoom / 2.f, 1.f - 1.f / m_Zoom / 2.f);
		zoomArea.Min = mouseUV - ImVec2{1.f / m_Zoom / 2.f, 1.f / m_Zoom / 2.f};
		zoomArea.Max = mouseUV + ImVec2{1.f / m_Zoom / 2.f, 1.f / m_Zoom / 2.f};
	}

	// todo - minimap - done
	//      - channels

	auto& activeRenderer = Core::C_Application::Get().GetActiveRenderer();
	if (!m_GUIHandle)
	{
		m_GUIHandle = activeRenderer.GetTextureGUIHandle(m_Texture);
	}

	ImGui::Image((void*)(intptr_t)(m_GUIHandle), drawAreaSz, zoomArea.Min, zoomArea.Max);
	for (auto overlay : m_Overlays)
	{
		const auto overlayHandle = activeRenderer.GetTextureGUIHandle(overlay);
		ImGui::SetCursorPos(canvasP0);
		ImGui::Image((void*)(intptr_t)(overlayHandle), drawAreaSz, zoomArea.Min, zoomArea.Max);
	}

	// === minimap ===
	// show only when zoomed and mouse over
	if (isHovered && m_Zoom != 1.f)
	{
		constexpr float offsetFromCorner = 10.f;
		constexpr float maxWidth		 = 250.f;
		const float		width			 = std::min(drawAreaSz.x / 5.f, maxWidth);
		const float		height			 = width * (m_Size.y / m_Size.x);
		const ImVec2	minimapDrawAreaSz(width, height);
		ImGui::SetCursorPos(canvasP0 + ImVec2(drawAreaSz.x - offsetFromCorner - minimapDrawAreaSz.x, offsetFromCorner));
		const ImVec2 screenSpacePos = ImGui::GetCursorScreenPos();
		ImGui::Image((void*)(intptr_t)(m_GUIHandle), minimapDrawAreaSz, {0, 0}, {1, 1}, {1, 1, 1, 1}, {1, 0, 0, 1});

		for (auto overlay : m_Overlays)
		{
			const auto overlayHandle = activeRenderer.GetTextureGUIHandle(overlay);
			ImGui::SetCursorPos(canvasP0 + ImVec2(drawAreaSz.x - offsetFromCorner - minimapDrawAreaSz.x, offsetFromCorner));
			ImGui::Image((void*)(intptr_t)(overlayHandle), minimapDrawAreaSz, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 }, { 1, 0, 0, 1 });
		}

		// rect
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		const ImVec2 minPos(width * zoomArea.Min.x, height * zoomArea.Min.y);
		const ImVec2 maxPos(width * zoomArea.Max.x, height * zoomArea.Max.y);

		window->DrawList->AddRect(screenSpacePos + minPos, screenSpacePos + maxPos, ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 0.f, 0.f, .5f)), 0.0f);
	}
	ImGui::SetCursorPos(canvasP0);
	ImGui::ItemSize(imageRect);
	return false;
}

//=================================================================================
void C_ImageViewer::SetSize(const glm::vec2 dim)
{
	m_Size = dim;
}

//=================================================================================
void C_ImageViewer::AddOverlay(Renderer::Handle<Renderer::Texture> texture)
{
	m_Overlays.emplace_back(texture);
}

//=================================================================================
void C_ImageViewer::PopOverlay()
{
	if (m_Overlays.empty() == false)
	{
		m_Overlays.pop_back();
	}
}

} // namespace GLEngine::GUI