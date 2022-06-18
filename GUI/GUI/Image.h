#pragma once

#include <GUI/GUIPart.h>

#include <Renderer/Textures/DeviceTexture.h>

namespace GLEngine::GUI {
class GUI_API_EXPORT C_Image : public I_GUIPart {
public:
	// needs to get texture handle one day
	explicit C_Image(Renderer::I_DeviceTexture& texture);
	virtual void Draw() const override;

	void SetSize(const glm::vec2 dim);

private:
	std::reference_wrapper<Renderer::I_DeviceTexture> m_texture;
	glm::vec2										  m_Size; //< Size of GUI, not the image
};

} // namespace GLEngine::GUI