#pragma once


#include <GLRenderer/GUI/GUIPart.h>

#include <GLRenderer/Textures/Texture.h>

namespace GLEngine {
namespace GLRenderer {
namespace Textures {
class C_Texture;
}

namespace GUI {
// This should look like bool value for user and has size of bool value
// @todo unit testing
class C_Image : public I_GUIPart {
public:
	C_Image(std::shared_ptr<Textures::C_Texture>& texture);
	virtual void Draw() const override;
private:
	std::shared_ptr<Textures::C_Texture> m_texture;
};

}}}