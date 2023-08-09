#pragma once

#include <GUI/GUIPart.h>

#include <Renderer/Resources/RenderResourceHandle.h>
#include <Renderer/Textures/DeviceTexture.h>

namespace GLEngine::GUI {
class GUI_API_EXPORT C_ImageViewer : public I_GUIPart {
public:
	explicit C_ImageViewer(Renderer::Handle<Renderer::Texture> texture);
	virtual void Draw() const override;

	void SetSize(const glm::vec2 dim);

private:
	Renderer::Handle<Renderer::Texture> m_texture;
	glm::vec2							m_Size; //< Size of GUI, not the image
	mutable float						m_Zoom;
	mutable void*						m_GUIHandle;
};
} // namespace GLEngine::GUI
