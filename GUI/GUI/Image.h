#pragma once

#include <GUI/GUIPart.h>

#include <Renderer/Resources/RenderResourceHandle.h>

namespace GLEngine::GUI {
class GUI_API_EXPORT C_Image : public I_GUIPart {
public:
	// needs to get texture handle one day
	explicit C_Image(Renderer::Handle<Renderer::Texture> handle);
	bool Draw() const override;

	void SetSize(const glm::vec2 dim);

private:
	Renderer::Handle<Renderer::Texture> m_Texture;
	glm::vec2							m_Size; //< Size of GUI, not the image
	mutable float						m_Zoom;

	void* m_GUIHandle;
};

} // namespace GLEngine::GUI