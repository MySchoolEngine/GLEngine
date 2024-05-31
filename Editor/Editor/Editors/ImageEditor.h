#pragma once

#include <Editor/EditorApi.h>

#include <Renderer/Textures/Storage/TextureLinearStorage.h>

#include <GUI/GUIWindow.h>
#include <GUI/ImageViewer.h>

namespace GLEngine::Renderer {
class I_DeviceTexture;
}

namespace GLEngine::Editor {

class EDITOR_API_EXPORT C_ImageEditor : public GUI::C_Window {
public:
	C_ImageEditor(GUID guid);
	~C_ImageEditor();
	virtual void Update() override;

protected:
	virtual void DrawComponents() const override;

private:
	Renderer::C_TextureViewStorageCPU<float>   m_Storage;
	GUI::C_ImageViewer*						   m_GUIImage;
	std::shared_ptr<Renderer::I_DeviceTexture> m_DeviceImage; // should not be owning ptr
	std::shared_ptr<Renderer::I_DeviceTexture> m_Background;  // should not be owning ptr

	bool m_bDone = false;
	bool m_bFinish = false;
};
} // namespace GLEngine::Editor
