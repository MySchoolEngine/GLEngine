#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/Image.h>

#include <GLRenderer/Textures/Texture.h>


namespace GLEngine {
namespace GLRenderer {
namespace GUI {

//=================================================================================
C_Image::C_Image(std::shared_ptr<Textures::C_Texture>& texture)
	: m_texture(texture)
{

}

//=================================================================================
void C_Image::Draw() const
{
	GLuint texture = m_texture->GetTexture();
	::ImGui::Image((void*)&(texture),
	{
		256,
		256
	},
	{ 0,1 }, { 1,0 });
}

}}}