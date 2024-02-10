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
	, m_WindowTitle(windowName)
	, m_BasePath(basePath)
	, m_fileType(fileType)
	, m_SuccessCallback(succesCallback)
{
}

//=================================================================================
C_FileDialogWindow::~C_FileDialogWindow()
{
}

//=================================================================================
void C_FileDialogWindow::Draw() const
{
	// display
	if (ImGuiFileDialog::Instance()->Display(m_WindowName))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk() == true)
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

//=================================================================================
void C_FileDialogWindow::SetTitle(const std::string& windowTitle)
{
	m_WindowTitle = windowTitle;
}

//=================================================================================
void C_FileDialogWindow::SetBasePath(const std::filesystem::path& basePath)
{
	m_BasePath = basePath;
}

//=================================================================================
void C_FileDialogWindow::OnSetVisible()
{
	ImGuiFileDialog::Instance()->OpenDialog(m_WindowName, m_WindowTitle, m_fileType.c_str(), m_BasePath.generic_string(), "");
}

//=================================================================================
void C_FileDialogWindow::OnHide()
{
	ImGuiFileDialog::Instance()->Close();
}

} // namespace GLEngine::GUI
