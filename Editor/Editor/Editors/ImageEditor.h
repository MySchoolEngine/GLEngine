#pragma once

#include <Editor/EditorApi.h>

#include <Renderer/Textures/Storage/TextureLinearStorage.h>

#include <GUI/GUIWindow.h>
#include <GUI/ImageViewer.h>

namespace GLEngine::Renderer {
class I_DeviceTexture;
class I_Renderer;
} // namespace GLEngine::Renderer

namespace GLEngine::Editor {
class C_ImageEditorTool;

class EDITOR_API_EXPORT C_ImageEditor : public GUI::C_Window {
public:
	C_ImageEditor(GUID guid, GUI::C_GUIManager& guiMGR);
	~C_ImageEditor() override;
	void Update() override;

protected:
	void DrawComponents() const override;

	bool ToggleHistogram();
	void SetupToolPreview();

private:
	void CreateTextures(Renderer::I_Renderer& renderer);

	Renderer::C_TextureViewStorageCPU<float> m_Storage;
	GUI::C_ImageViewer						 m_GUIImage; // view
	GUI::Menu::C_Menu						 m_FileMenu;
	GUI::Menu::C_Menu						 m_Tools;
	Renderer::Handle<Renderer::Texture>		 m_DeviceImage;
	Renderer::Handle<Renderer::Texture>		 m_Background;


	std::unique_ptr<C_ImageEditorTool> m_ActiveTool;

	bool		 m_bDone	 = false;
	mutable bool m_bFinish	 = false;
	bool		 m_bModified = false;
};
} // namespace GLEngine::Editor
