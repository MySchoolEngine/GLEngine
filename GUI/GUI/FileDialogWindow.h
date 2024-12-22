#pragma once

#include <GUI/GUIWindow.h>

namespace GLEngine::GUI {
class GUI_API_EXPORT C_FileDialogWindow : public C_Window {
public:
	explicit C_FileDialogWindow(const std::string&														fileType,
								const std::string&														windowName,
								const std::function<void(const std::filesystem::path&, C_GUIManager&)>& successCallback,
								GUID																	guid,
								const std::filesystem::path&											basePath = "."); // data folder by default
	~C_FileDialogWindow() override;
	virtual bool Draw(C_GUIManager& guiMgr) const override;

	void SetTitle(const std::string& windowTitle);
	void SetBasePath(const std::filesystem::path& basePath);

private:
	std::string										  m_WindowName;
	std::string										  m_WindowTitle;
	std::filesystem::path							  m_BasePath;
	std::string										  m_fileType;
	std::function<void(const std::filesystem::path&, C_GUIManager&)> m_SuccessCallback;

protected:
	virtual void OnSetVisible() override;
	virtual void OnHide() override;
};

} // namespace GLEngine::GUI
