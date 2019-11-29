#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/ConsoleWindow.h>

#include <GLRenderer/GUI/GUIPart.h>

#include <Utils/Logging/Filter.h>

#include <fmt/format.h>

namespace GLEngine::GLRenderer::GUI {

std::array<ImVec4, 4> C_ConsoleWindow::s_LevelColors
= {
	ImVec4(1,0,0,1),
	ImVec4(1,0.8f,0,1),
	ImVec4(1,1,1,1),
	ImVec4(0.7f,0.7f,0.7f,1.0)
};

//=================================================================================
C_ConsoleWindow::C_ConsoleWindow(GUID guid)
	: C_Window(guid, "Console")
	, m_LastLogIndex(m_Logs.size()-1)
{
	Utils::Logging::C_LoggingSystem::Instance().AddLogger(this);
}

//=================================================================================
C_ConsoleWindow::~C_ConsoleWindow()
{

	Utils::Logging::C_LoggingSystem::Instance().RemoveLogger(this);
}

//=================================================================================
void C_ConsoleWindow::Log(Utils::Logging::E_Level level, Utils::Logging::E_Context context, int line, const char* file, const std::string& text)
{
	m_LastLogIndex++;
	m_LastLogIndex %= m_Logs.size();
	m_Logs[m_LastLogIndex] = Utils::Logging::S_Data {text, file, line, level, context};
}

//=================================================================================
void C_ConsoleWindow::Draw() const
{
	::ImGui::Begin(m_Name.c_str(), &m_IsVisible);
	static const char* levels[] = {"Error", "Warning", "Info", "Debug", "None"};
	static const char* current_level = levels[4];
	static int selectedLevelId = 4;
	using levelUnderlying = std::underlying_type_t<Utils::Logging::E_Level>;

	static std::unique_ptr<Utils::Logging::C_Filter> filter = std::make_unique<Utils::Logging::C_PassAllFilter>();

	if (::ImGui::BeginCombo("##level", current_level))
	{
		for (int n = 0; n < sizeof(levels)/sizeof(char*); n++)
		{
			bool is_selected = (current_level == levels[n]);
			if (ImGui::Selectable(levels[n], is_selected))
			{
				current_level = levels[n];
				selectedLevelId = n;
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		::ImGui::EndCombo();
	}



	if (selectedLevelId != 4) {
		using namespace Utils::Logging;
		filter = std::make_unique<C_NegationFilter>(new C_LevelFilter(static_cast<E_Level>(selectedLevelId)));
	}
	else
	{
		filter = std::make_unique<Utils::Logging::C_PassAllFilter>();
	}

	::ImGui::BeginChild("Output");
	for (std::size_t i = ((m_LastLogIndex + 1) % m_Logs.size()); i != m_LastLogIndex; i = ((i + 1) % m_Logs.size()))
	{
		auto& data = m_Logs[i];

		if (data.m_Text.empty()) {
			continue;
		}
		if (!filter->Filter(data)) continue;
			::ImGui::TextColored(s_LevelColors[static_cast<levelUnderlying>(data.m_Level)], data.m_Text.c_str());
	}
	::ImGui::SetScrollHereY(1.0f);
	::ImGui::EndChild();
	::ImGui::End();
}

}