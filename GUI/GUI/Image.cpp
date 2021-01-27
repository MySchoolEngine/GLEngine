#include <GUIStdafx.h>

#include <GUI/Image.h>

//#include <GLRenderer/Textures/Texture.h>


namespace GLEngine::GUI {

//=================================================================================
C_Image::C_Image(const std::shared_ptr<GLRenderer::Textures::C_Texture>& texture)
	: m_texture(texture)
{
}

//=================================================================================
void C_Image::Draw() const
{
	// GLuint texture = m_texture->GetTexture();
	//::ImGui::Image((void*)&(texture),
	//{
	//	256,
	//	256
	//},
	//{ 0,1 }, { 1,0 });
}

} // namespace GLEngine::GUI