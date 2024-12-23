#include <GUIStdafx.h>

#include <GUI/FileDialogWindow.h>

#include <ImGuiFileDialog.h>

namespace GLEngine::GUI {

//=================================================================================
C_FileDialogWindow::C_FileDialogWindow(std::string															   fileType,
									   const std::string&													   windowName,
									   const std::function<void(const std::filesystem::path&, C_GUIManager&)>& successCallback,
									   const GUID																	   guid,
									   std::filesystem::path												   basePath)
	: C_Window(guid, windowName)
	, m_WindowName(windowName)
	, m_WindowTitle(windowName)
	, m_BasePath(std::move(basePath))
	, m_FileType(std::move(fileType))
	, m_SuccessCallback(successCallback)
{
}

//=================================================================================
C_FileDialogWindow::~C_FileDialogWindow() = default;

//=================================================================================
bool C_FileDialogWindow::Draw(C_GUIManager& guiMgr) const
{
	// display
	if (ImGuiFileDialog::Instance()->Display(m_WindowName))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk() == true)
		{
			const std::filesystem::path filePathName = std::filesystem::relative(ImGuiFileDialog::Instance()->GetFilePathName());
			m_SuccessCallback(filePathName, guiMgr);
		}
		else
		{
			m_WantToBeDestroyed = true;
		}
	}
	if (m_WantToBeDestroyed)
		ImGuiFileDialog::Instance()->Close();
	return false; // todo
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
	ImGuiFileDialog::Instance()->OpenDialog(m_WindowName, m_WindowTitle, m_FileType.c_str(), m_BasePath.generic_string(), "");
}

//=================================================================================
void C_FileDialogWindow::OnHide()
{
	ImGuiFileDialog::Instance()->Close();
}

} // namespace GLEngine::GUI
