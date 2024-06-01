#pragma once

#include <Editor/EditorApi.h>

#include <Renderer/Textures/Storage/TextureLinearStorage.h>

#include <GUI/GUIWindow.h>
#include <GUI/ImageViewer.h>

namespace GLEngine::Renderer {
class I_DeviceTexture;
}

namespace GLEngine::Editor {
class C_ImageEditorTool;

class EDITOR_API_EXPORT C_ImageEditor : public GUI::C_Window {
public:
	C_ImageEditor(GUID guid, GUI::C_GUIManager& guiMGR);
	~C_ImageEditor();
	virtual void Update() override;

protected:
	virtual void DrawComponents() const override;

	void ToggleHistogram();

private:
	Renderer::C_TextureViewStorageCPU<float>   m_Storage;
	GUI::C_ImageViewer*						   m_GUIImage; // view
	GUI::Menu::C_Menu						   m_Tools;
	std::shared_ptr<Renderer::I_DeviceTexture> m_DeviceImage; // should not be owning ptr
	std::shared_ptr<Renderer::I_DeviceTexture> m_Background;  // should not be owning ptr


	std::unique_ptr<C_ImageEditorTool> m_ActiveTool;

	bool		 m_bDone	 = false;
	mutable bool m_bFinish	 = false;
	bool		 m_bModified = false;
};
} // namespace GLEngine::Editor
