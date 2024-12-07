#pragma once

#include <GUI/GUIWindow.h>

#include <Utils/Logging/Data.h>
#include <Utils/Logging/ILogger.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

namespace GLEngine::GUI
{
class GUI_API_EXPORT C_ConsoleWindow : public C_Window, public Utils::Logging::I_Logger
{
public:
	explicit C_ConsoleWindow(GUID guid);
	virtual ~C_ConsoleWindow();

	void Log(::Utils::Logging::E_Level level, ::Utils::Logging::E_Context context, int line, const char* file, const std::string& text) override;
	bool Draw(C_GUIManager& guiMgr) const override;

private:
	//@todo replace with stack vector once we have 'em
	std::array<Utils::Logging::S_Data, 1000> m_Logs;
	std::size_t								 m_LastLogIndex;
	std::optional<Utils::Logging::E_Level>	 m_Level;
	std::optional<Utils::Logging::E_Context> m_Context;

	static std::array<ImVec4, 4> s_LevelColors;
	RTTR_ENABLE();
	RTTR_REGISTRATION_FRIEND;
};

} // namespace GLEngine::GUI