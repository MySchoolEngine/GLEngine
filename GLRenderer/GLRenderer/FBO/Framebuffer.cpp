#include <GLRendererStdafx.h>

#include <GLRenderer/FBO/Framebuffer.h>
#include <GLRenderer/Textures/Texture.h>

namespace GLEngine {
namespace GLRenderer {
namespace Components {


//=================================================================================
C_Framebuffer::C_Framebuffer()
{
	glGenFramebuffers(1, &m_FBO);

}

//=================================================================================
void C_Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

//=================================================================================
void C_Framebuffer::Unbind()
{
	// switch back to window-system-provided framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//=================================================================================
void C_Framebuffer::AttachTexture(GLenum attachement, std::shared_ptr<Textures::C_Texture> texture)
{
	if (m_attachements.find(attachement) != m_attachements.end()) {
		std::cerr << "Attaching multiple textures to one attach point in framebuffer" << std::endl;
	}
	Bind();
	glFramebufferTexture(GL_FRAMEBUFFER, attachement, texture->GetTexture(), 0);
	Unbind();

	m_attachements[attachement] = texture;
}

//=================================================================================
std::shared_ptr<Textures::C_Texture> C_Framebuffer::GetAttachement(GLenum attachement)
{
	GLE_ASSERT(m_attachements.find(attachement) != m_attachements.end(), "There is no attachement of type {}", attachement);
	return m_attachements[attachement];
}

}}}