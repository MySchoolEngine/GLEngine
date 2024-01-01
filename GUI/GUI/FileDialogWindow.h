#pragma once

#include <GUI/GUIWindow.h>

namespace GLEngine::GUI {
class GUI_API_EXPORT C_FileDialogWindow : public C_Window {
public:
	explicit C_FileDialogWindow(const std::string&										 fileType,
								const std::string&										 windowName,
								const std::function<void(const std::filesystem::path&)>& succesCallback,
								GUID													 guid,
								const std::filesystem::path&							 basePath = "."); // data folder by default
	virtual ~C_FileDialogWindow();
	virtual bool Draw() const override;

private:
	std::string										  m_WindowName;
	std::function<void(const std::filesystem::path&)> m_SuccessCallback;
};

} // namespace GLEngine::GUI
