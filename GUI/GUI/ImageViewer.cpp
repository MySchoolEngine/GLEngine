#include <GUIStdafx.h>

#include <GUI/ImageViewer.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Resources/ResourceManager.h>

#include <Core/Application.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace GLEngine::GUI {
// constants
namespace {
constexpr ImVec2 s_textureRect{380.f, 88.f};
constexpr ImVec2 s_texturePreviewSize{s_textureRect.y - 4.f, s_textureRect.y - 4.f};
} // namespace

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
		m_Zoom		  = std::clamp(m_Zoom + io.MouseWheel / 10.f, 1.f, 10.f);
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
	if (m_Background.Handle.IsValid())
	{
		ImRect	   scaledZoomArea(ImVec2{zoomArea.Min.x / m_Background.Scale.x, zoomArea.Min.y / m_Background.Scale.y},
							  ImVec2{zoomArea.Max.x / m_Background.Scale.x, zoomArea.Max.y / m_Background.Scale.y});
		const auto backgroundHandle = activeRenderer.GetTextureGUIHandle(m_Background.Handle);
		ImGui::Image((void*)(intptr_t)(backgroundHandle), drawAreaSz, scaledZoomArea.Min, scaledZoomArea.Max);
	}

	if (!m_GUIHandle)
	{
		m_GUIHandle = activeRenderer.GetTextureGUIHandle(m_Texture);
	}

	ImGui::SetCursorPos(canvasP0);
	if (m_bShowImage)
		ImGui::Image((void*)(intptr_t)(m_GUIHandle), drawAreaSz, zoomArea.Min, zoomArea.Max);
	for (auto [overlay, scale, flipVertically, Show] : m_Overlays)
	{
		if (!Show)
			continue;
		const auto overlayHandle = activeRenderer.GetTextureGUIHandle(overlay);
		ImGui::SetCursorPos(canvasP0);
		DrawImageWithCorrectAxis(overlayHandle, drawAreaSz, zoomArea, flipVertically);
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

		if (m_Background.Handle.IsValid())
		{
			ImRect	   scaledZoomArea({0, 0}, ImVec2{1.f / m_Background.Scale.x, 1.f / m_Background.Scale.y});
			const auto backgroundHandle = activeRenderer.GetTextureGUIHandle(m_Background.Handle);
			ImGui::Image((void*)(intptr_t)(backgroundHandle), minimapDrawAreaSz, scaledZoomArea.Min, scaledZoomArea.Max);
		}


		ImGui::SetCursorPos(canvasP0 + ImVec2(drawAreaSz.x - offsetFromCorner - minimapDrawAreaSz.x, offsetFromCorner));
		ImVec4 tintColor{1, 1, 1, 1};
		if (!m_bShowImage)
			tintColor.w = 0; // hides the image, but still allows to draw the outline
		ImGui::Image((void*)(intptr_t)(m_GUIHandle), minimapDrawAreaSz, {0, 0}, {1, 1}, tintColor, {1, 0, 0, 1});

		for (auto [overlay, scale, flipVertically, Show] : m_Overlays)
		{
			if (!Show)
				continue;
			const auto overlayHandle = activeRenderer.GetTextureGUIHandle(overlay);
			ImGui::SetCursorPos(canvasP0 + ImVec2(drawAreaSz.x - offsetFromCorner - minimapDrawAreaSz.x, offsetFromCorner));
			DrawImageWithCorrectAxis(overlayHandle, minimapDrawAreaSz, ImRect(ImVec2{0, 0}, ImVec2{1, 1}), flipVertically);
		}

		// rect
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		const ImVec2 minPos(width * zoomArea.Min.x, height * zoomArea.Min.y);
		const ImVec2 maxPos(width * zoomArea.Max.x, height * zoomArea.Max.y);

		window->DrawList->AddRect(screenSpacePos + minPos, screenSpacePos + maxPos, ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 0.f, 0.f, .5f)), 0.0f);
	}
	if (m_bShowLayers)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{0, 0});

		// Set pos instead of same line
		ImGui::SetCursorPos(canvasP0 + ImVec2{m_Size.x + 5, 0});
		// should have some kind of uniqueness there
		const ImVec2 SideBarSize{390.f, drawAreaSz.y};
		ImGui::BeginChildFrame(ImGuiID{static_cast<unsigned int>(std::hash<std::string>{}("TextureViewerLayers_ToolBar"))}, SideBarSize);

		DrawLayerBox(m_Texture, m_bShowImage, activeRenderer);
		for (auto& overlayDefinition : m_Overlays)
		{
			DrawLayerBox(overlayDefinition.Handle, overlayDefinition.bShow, activeRenderer);
		}
		ImGui::EndChildFrame();
		ImGui::PopStyleVar(2);
	}
	ImGui::SetCursorPos(canvasP0);
	if (!m_bShowLayers)
	{
		ImGui::ItemSize(imageRect); // should have the width of show layers as well
	}
	else
	{
		ImRect elementSizeWithLayers = imageRect;
		elementSizeWithLayers.Max += {s_textureRect.x, 0};
		ImGui::ItemSize(elementSizeWithLayers);
	}
	return false;
}

//=================================================================================
void C_ImageViewer::SetSize(const glm::vec2 dim)
{
	m_Size = dim;
}

//=================================================================================
void C_ImageViewer::AddOverlay(Renderer::Handle<Renderer::Texture> texture, bool flipVertically)
{
	m_Overlays.emplace_back(texture, glm::vec2(1.f, 1.f), flipVertically);
}

//=================================================================================
void C_ImageViewer::PopOverlay()
{
	if (m_Overlays.empty() == false)
	{
		m_Overlays.pop_back();
	}
}

//=================================================================================
void C_ImageViewer::ShowLayers(bool show)
{
	m_bShowLayers = show;
}

//=================================================================================
void C_ImageViewer::DrawLayerBox(Renderer::Handle<Renderer::Texture> handle, bool& show, Renderer::I_Renderer& activeRenderer)
{
	auto& resMGR	= activeRenderer.GetRM();
	auto* layerDesc = resMGR.getDescriptor(handle);
	if (ImGui::BeginChildFrame(ImGuiID{static_cast<unsigned int>(std::hash<std::string>{}("TextureViewerLayers_" + layerDesc->name))}, s_textureRect))
	{
		ImGui::SetCursorPos(ImVec2{2, 2});
		const auto overlayHandle = activeRenderer.GetTextureGUIHandle(handle);
		ImGui::BeginChildFrame(static_cast<unsigned int>(std::hash<std::string>{}("TextureViewerLayers_Preview_" + layerDesc->name)), s_texturePreviewSize);
		ImGui::Image((void*)(intptr_t)(overlayHandle), s_texturePreviewSize, {0, 0}, {1, 1}, {1, 1, 1, 1});
		ImGui::EndChildFrame();
		ImGui::SameLine();
		ImGui::Checkbox("Show", &show);
	}
	ImGui::EndChildFrame();
}

//=================================================================================
void C_ImageViewer::DrawImageWithCorrectAxis(void* const imageGPUHandle, const ImVec2& drawArea, const ImRect& rect, bool flipVertically)
{
	if (flipVertically)
	{
		ImGui::Image((void*)(intptr_t)(imageGPUHandle), drawArea, {rect.Min.x, 1.f - rect.Min.y}, {rect.Max.x, 1.f - rect.Max.y});
	}
	else
	{
		ImGui::Image((void*)(intptr_t)(imageGPUHandle), drawArea, rect.Min, rect.Max);
	}
}

//=================================================================================
void C_ImageViewer::SetBackground(Renderer::Handle<Renderer::Texture> texture, const glm::vec2& scale)
{
	m_Background = {
		.Handle			 = texture,
		.Scale			 = scale,
		.bFlipVertically = false,
	};
}
} // namespace GLEngine::GUI