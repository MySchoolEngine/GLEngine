#pragma once

#include <GUI/GUIPart.h>

#include <Renderer/Textures/DeviceTexture.h>

namespace GLEngine::GUI {
class GUI_API_EXPORT C_Texture : public I_GUIPart {
public:
	C_Texture(std::weak_ptr<Renderer::I_DeviceTexture> texture, const glm::uvec2& dim = {256, 256});

	virtual void Draw() const override;

	void SetOnTextureCleanCB(std::function<void()> cb);

private:
	std::weak_ptr<Renderer::I_DeviceTexture> m_Texture;
	glm::uvec2								 m_Dimensions;
	std::function<void()>					 m_TextureCleanCB;
};
} // namespace GLEngine::GUI
