#include <GUIStdAfx.h>

#include <GUI/GUIUtils.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace GLEngine::GUI {

//=================================================================================
bool DrawSquareButton(ImDrawList* draw_list, const ImVec2 pos, const E_ButtonType type)
{
	const ImGuiIO& io = ImGui::GetIO();
	const ImRect   rect(pos, ImVec2(pos.x + 16, pos.y + 16));
	const auto	   mouseOver = rect.Contains(io.MousePos);
	const auto	   color   = mouseOver ? 0xFFAAAAAA : 0x50000000;
	const auto	   midy	   = pos.y + 16 / 2 - 0.5f;
	const auto	   midx	   = pos.x + 16 / 2 - 0.5f;
	draw_list->AddRect(rect.Min, rect.Max, color, 4);

	switch (type)
	{
	case E_ButtonType::Plus:
		draw_list->AddLine(ImVec2(midx, rect.Min.y + 3), ImVec2(midx, rect.Max.y - 3), color, 2);
		[[fallthrough]];
	case E_ButtonType::Minus:
		draw_list->AddLine(ImVec2(rect.Min.x + 3, midy), ImVec2(rect.Max.x - 3, midy), color, 2);
		break;
	case E_ButtonType::Cross:
		draw_list->AddLine(ImVec2(rect.Min.x + 3, rect.Min.y + 3), ImVec2(rect.Max.x - 3, rect.Max.y - 3), color, 2);
		draw_list->AddLine(ImVec2(rect.Max.x - 3, rect.Min.y + 3), ImVec2(rect.Min.x + 3, rect.Max.y - 3), color, 2);
		break;
	}

	if (mouseOver) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
	}

	return mouseOver;
}

} // namespace GLEngine::GUI
