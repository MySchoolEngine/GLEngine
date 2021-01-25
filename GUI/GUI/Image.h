#pragma once

#include <GUI/GUIPart.h>

namespace GLEngine::GLRenderer::Textures {
class C_Texture;
}

namespace GLEngine::GUI {
// This should look like bool value for user and has size of bool value
// @todo unit testing
class C_Image : public I_GUIPart {
public:
	explicit C_Image(const std::shared_ptr<GLRenderer::Textures::C_Texture>& texture);
	virtual void Draw() const override;

private:
	std::shared_ptr<GLRenderer::Textures::C_Texture> m_texture;
};

} // namespace GLEngine::GUI