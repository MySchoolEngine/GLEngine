#include <GUIStdAfx.h>

#include <GUI/FileDialogWindow.h>

#include <ImGuiFileDialog/ImGuiFileDialog.h>

namespace GLEngine::GUI {

//=================================================================================
C_FileDialogWindow::C_FileDialogWindow(const std::string&										fileType,
									   const std::string&										windowName,
									   const std::function<void(const std::filesystem::path&)>& succesCallback,
									   GUID														guid,
									   const std::filesystem::path&								basePath)
	: C_Window(guid, windowName)
	, m_WindowName(windowName)
	, m_SuccessCallback(succesCallback)
{
	igfd::ImGuiFileDialog::Instance()->OpenDialog(m_WindowName, windowName.c_str(), fileType.c_str(), basePath.generic_string(), "");
}

//=================================================================================
C_FileDialogWindow::~C_FileDialogWindow()
{
	igfd::ImGuiFileDialog::Instance()->CloseDialog(m_WindowName);
}

//=================================================================================
void C_FileDialogWindow::Draw() const
{
	// display
	if (igfd::ImGuiFileDialog::Instance()->FileDialog(m_WindowName))
	{
		// action if OK
		if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
		{
			const std::filesystem::path filePathName = igfd::ImGuiFileDialog::Instance()->GetFilePathName();
			const std::filesystem::path filePath	 = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
			m_SuccessCallback(filePath / filePathName);
		}
		else
		{
			//CORE_LOG(E_Level::Error, E_Context::Core, "Not ok");
		}
	}
}

} // namespace GLEngine::GUI
