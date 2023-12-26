#include <GUIStdafx.h>

#include <GUI/FileDialogWindow.h>

#include <ImGuiFileDialog.h>

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
	ImGuiFileDialog::Instance()->OpenDialog(m_WindowName, windowName, fileType.c_str(), basePath.generic_string(), "");
}

//=================================================================================
C_FileDialogWindow::~C_FileDialogWindow()
{
	ImGuiFileDialog::Instance()->Close();
}

//=================================================================================
void C_FileDialogWindow::Draw() const
{
	// display
	if (ImGuiFileDialog::Instance()->Display(m_WindowName))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk()== true)
		{
			const std::filesystem::path filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			const std::filesystem::path filePath	 = ImGuiFileDialog::Instance()->GetCurrentPath();
			m_SuccessCallback(filePath / filePathName);
		}
		else
		{
			m_WantToBeDestroyed = true;
		}
	}
}

} // namespace GLEngine::GUI
