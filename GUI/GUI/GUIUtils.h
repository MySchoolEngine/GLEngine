#pragma once

#include <GUI/GUIApi.h>

struct ImVec2;
struct ImDrawList;

namespace GLEngine::GUI {
enum class E_ButtonType
{
	Plus,
	Minus,
	Cross
};
// Returns true if mouse over button
GUI_API_EXPORT [[nodiscard]] bool DrawSquareButton(ImDrawList* draw_list, const ImVec2 pos, const E_ButtonType type);
GUI_API_EXPORT [[nodiscard]] float Splitter();
}

