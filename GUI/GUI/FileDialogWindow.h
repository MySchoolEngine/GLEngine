#pragma once

#include <GUI/GUIWindow.h>

namespace GLEngine::GUI {
class GUI_API_EXPORT C_FileDialogWindow : public C_Window {
public:
	explicit C_FileDialogWindow(std::string																fileType,
								const std::string&														windowName,
								const std::function<void(const std::filesystem::path&, C_GUIManager&)>& successCallback,
								GUID																	guid,
								std::filesystem::path													basePath = "."); // data folder by default
	~C_FileDialogWindow() override;
	C_FileDialogWindow(const C_FileDialogWindow&) = delete;
	C_FileDialogWindow(C_FileDialogWindow&&)	  = delete;
	void operator=(const C_FileDialogWindow&)	  = delete;
	void operator=(C_FileDialogWindow&&)		  = delete;

	bool Draw(C_GUIManager& guiMgr) const override;

	void SetTitle(const std::string& windowTitle);
	void SetBasePath(const std::filesystem::path& basePath);

private:
	std::string														 m_WindowName;
	std::string														 m_WindowTitle;
	std::filesystem::path											 m_BasePath;
	std::string														 m_FileType;
	std::function<void(const std::filesystem::path&, C_GUIManager&)> m_SuccessCallback;

protected:
	void OnSetVisible() override;
	void OnHide() override;
};

} // namespace GLEngine::GUI
