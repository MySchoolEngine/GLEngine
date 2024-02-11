#include <GUIStdafx.h>

#include <GUI/ConsoleWindow.h>
#include <GUI/GUIPart.h>

#include <Utils/Logging/Filter.h>

#include <fmt/format.h>

namespace GLEngine::GUI {

std::array<ImVec4, 4> C_ConsoleWindow::s_LevelColors = {ImVec4(1, 0, 0, 1), ImVec4(1, 0.8f, 0, 1), ImVec4(1, 1, 1, 1), ImVec4(0.7f, 0.7f, 0.7f, 1.0)};

//=================================================================================
C_ConsoleWindow::C_ConsoleWindow(GUID guid)
	: C_Window(guid, "Console")
	, m_LastLogIndex(m_Logs.size() - 1)
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
	m_Logs[m_LastLogIndex] = Utils::Logging::S_Data{text, file, line, level, context};
}

//=================================================================================
bool C_ConsoleWindow::Draw() const
{
	::ImGui::Begin(m_Name.c_str(), &m_IsVisible);
	static const char* levels[]			 = {"Error", "Warning", "Info", "Debug", "None"};
	static const char* contexts[]		 = {"Core", "Render", "Entity", "None"};
	static const char* current_level	 = levels[4];
	static const char* current_context	 = contexts[3];
	static int		   selectedLevelId	 = 4;
	static int		   selectedContextId = 3;
	using levelUnderlying				 = std::underlying_type_t<Utils::Logging::E_Level>;
	using contextUnderlying				 = std::underlying_type_t<Utils::Logging::E_Context>;

	static char m_SpawningName[255] = {0};

	static std::unique_ptr<Utils::Logging::C_Filter> filter = std::make_unique<Utils::Logging::C_PassAllFilter>();

	if (::ImGui::BeginCombo("##level", current_level))
	{
		for (int n = 0; n < sizeof(levels) / sizeof(char*); n++)
		{
			bool is_selected = (current_level == levels[n]);
			if (::ImGui::Selectable(levels[n], is_selected))
			{
				current_level	= levels[n];
				selectedLevelId = n;
			}
			if (is_selected)
			{
				::ImGui::SetItemDefaultFocus();
			}
		}
		::ImGui::EndCombo();
	}


	if (::ImGui::BeginCombo("##context", current_context))
	{
		for (int n = 0; n < sizeof(contexts) / sizeof(char*); n++)
		{
			bool is_selected = (current_context == contexts[n]);
			if (::ImGui::Selectable(contexts[n], is_selected))
			{
				current_context	  = contexts[n];
				selectedContextId = n;
			}
			if (is_selected)
			{
				::ImGui::SetItemDefaultFocus();
			}
		}
		::ImGui::EndCombo();
	}


	::ImGui::InputText("Filtered text", m_SpawningName, 255);

	Utils::Logging::C_Filter* innerFilter(nullptr);

	if (selectedContextId != 3)
	{
		using namespace Utils::Logging;
		innerFilter = new C_NegationFilter(new C_ContextFilter(static_cast<E_Context>(selectedContextId)));
	}

	if (strlen(m_SpawningName) != 0)
	{
		using namespace Utils::Logging;
		innerFilter = new C_TextFilter(m_SpawningName, innerFilter);
	}


	if (selectedLevelId != 4)
	{
		using namespace Utils::Logging;
		filter = std::make_unique<C_NegationFilter>(new C_LevelFilter(static_cast<E_Level>(selectedLevelId), innerFilter));
	}
	else
	{
		using namespace Utils::Logging;
		if (!innerFilter)
			filter = std::make_unique<Utils::Logging::C_PassAllFilter>();
		else
			filter = std::unique_ptr<C_Filter>(innerFilter);
	}

	::ImGui::BeginChild("Output");
	for (std::size_t i = ((m_LastLogIndex + 1) % m_Logs.size()); i != m_LastLogIndex; i = ((i + 1) % m_Logs.size()))
	{
		auto& data = m_Logs[i];

		if (data.m_Text.empty())
		{
			continue;
		}
		if (!filter->Filter(data))
			continue;
		::ImGui::TextColored(s_LevelColors[static_cast<levelUnderlying>(data.m_Level)], "%s", data.m_Text.c_str());
	}
	::ImGui::SetScrollHereY(1.0f);
	::ImGui::EndChild();
	::ImGui::End();
	return false;
}

} // namespace GLEngine::GUI