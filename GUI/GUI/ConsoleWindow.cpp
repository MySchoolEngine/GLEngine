#include <GUIStdafx.h>

#include <GUI/ConsoleWindow.h>

#include <GUI/ReflectionGUI.h>

#include <Utils/Logging/Filter.h>
#include <Utils/Reflection/Metadata.h>

#if _WIN32
#include <shellapi.h>
#endif

#include <fmt/format.h>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::GUI;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_ConsoleWindow>("C_ConsoleWindow")
		.property("Level", &C_ConsoleWindow::m_Level)
		(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::EnumSelectOptional>(),
			RegisterMetamember<UI::EnumSelectOptional::Name>("Level:"),
			RegisterMetamember<UI::EnumSelectOptional::OptionalName>("All"),
			RegisterMetamember<SerializationCls::NoSerialize>(true)
		)
		.property("Context", &C_ConsoleWindow::m_Context)
		(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::EnumSelectOptional>(),
			RegisterMetamember<UI::EnumSelectOptional::Name>("Context:"),
			RegisterMetamember<UI::EnumSelectOptional::OptionalName>("All"),
			RegisterMetamember<SerializationCls::NoSerialize>(true)
		);

	rttr::registration::class_<std::optional<Utils::Logging::E_Level>>("std::optional<Utils::Logging::E_Level>");
	rttr::registration::class_<std::optional<Utils::Logging::E_Level>>("std::optional<Utils::Logging::E_Context>");
}
// clang-format on

namespace rttr
{
template<typename T>
struct wrapper_mapper<std::optional<T>>
{
	using wrapped_type = const T*;
	using type = std::optional<T>;

	static RTTR_INLINE const wrapped_type get(const type& obj)
	{
		if (obj.has_value())
			return &obj.value();
		return nullptr;
	}

	static RTTR_INLINE type create(const wrapped_type& t)
	{
		if (!t)
			return std::nullopt;
		return type(*t);
	}
};
}

namespace GLEngine::GUI
{

std::array<ImVec4, 4> C_ConsoleWindow::s_LevelColors = { ImVec4(1, 0, 0, 1), ImVec4(1, 0.8f, 0, 1), ImVec4(1, 1, 1, 1), ImVec4(0.7f, 0.7f, 0.7f, 1.0) };

//=================================================================================
C_ConsoleWindow::C_ConsoleWindow(const GUID guid)
	: C_Window(guid, "Console")
	, m_LastLogIndex(m_Logs.size() - 1)
	, m_Level(Utils::Logging::E_Level::Info)
{
	Utils::Logging::C_LoggingSystem::Instance().AddLogger(this);
}

//=================================================================================
C_ConsoleWindow::~C_ConsoleWindow()
{
	Utils::Logging::C_LoggingSystem::Instance().RemoveLogger(this);
}

//=================================================================================
void C_ConsoleWindow::Log(const Utils::Logging::E_Level level, const Utils::Logging::E_Context context, const int line, const char* file, const std::string& text)
{
	m_LastLogIndex++;
	m_LastLogIndex %= m_Logs.size();
	m_Logs[m_LastLogIndex] = Utils::Logging::S_Data{.m_Text = text, .m_File = file, .m_Line = line, .m_Level = level, .m_Context = context};
}

//=================================================================================
bool C_ConsoleWindow::Draw(C_GUIManager& guiMgr) const
{
	::ImGui::Begin(m_Name.c_str(), &m_IsVisible);
	rttr::instance obj(*this);
	if (GUI::DrawAllPropertyGUI(obj).empty() == false)
	{
		//m_Changed = true;
	}
	using levelUnderlying = std::underlying_type_t<Utils::Logging::E_Level>;
	static char filteredText[255] = { 0 };

	static std::unique_ptr<Utils::Logging::C_Filter> filter = std::make_unique<Utils::Logging::C_PassAllFilter>();

	::ImGui::InputText("Filtered text", filteredText, 255);

	Utils::Logging::C_Filter* innerFilter(nullptr);

	if (m_Context.has_value())
	{
		using namespace Utils::Logging;
		innerFilter = new C_NegationFilter(new C_ContextFilter(static_cast<E_Context>(m_Context.value())));
	}

	if (strlen(filteredText) != 0)
	{
		using namespace Utils::Logging;
		innerFilter = new C_TextFilter(filteredText, innerFilter);
	}


	if (m_Level.has_value())
	{
		using namespace Utils::Logging;
		filter = std::make_unique<C_NegationFilter>(new C_LevelFilter(static_cast<E_Level>(m_Level.value()), innerFilter));
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
	std::size_t i = ((m_LastLogIndex + 1) % m_Logs.size());
	do
	{
		auto& data = m_Logs[i];

		if (!data.m_Text.empty() && filter->Filter(data))
		{
			::ImGui::TextColored(s_LevelColors[static_cast<levelUnderlying>(data.m_Level)], data.m_Text.c_str());
			if (::ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
				const auto linkText = data.m_File + ":" + std::to_string(data.m_Line);
				::ImGui::SetTooltip(linkText.c_str());
#if _WIN32
				if(::ImGui::IsItemClicked())
					ShellExecute(0, 0, std::wstring(data.m_File.begin(), data.m_File.end()).c_str(), 0, 0, SW_SHOW);
#endif
			}

		}
		i = ((i + 1) % m_Logs.size());
	} while (i != m_LastLogIndex + 1);
	::ImGui::SetScrollHereY(1.0f);
	::ImGui::EndChild();
	::ImGui::End();
	return false;
}

} // namespace GLEngine::GUI