#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/FBO/glFramebufferTexture.h>
#include <GLRenderer/FBO/Framebuffer.h>
#include <GLRenderer/Textures/Texture.h>

namespace GLEngine::GLRenderer {

//=================================================================================
C_Framebuffer::C_Framebuffer(const std::string& name)
	: m_DirtyFlag(true)
{
	glGenFramebuffers(1, &m_FBO);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glObjectLabel(GL_FRAMEBUFFER, m_FBO, static_cast<GLsizei>(name.size()), name.c_str());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//=================================================================================
C_Framebuffer::~C_Framebuffer()
{
	glDeleteBuffers(1, &m_FBO);
}

//=================================================================================
std::shared_ptr<Textures::C_Texture> C_Framebuffer::GetAttachement(GLenum attachement)
{
	GLE_ASSERT(m_attachements.find(attachement) != m_attachements.end(), "There is no attachement of type {}", attachement);
	return m_attachements[attachement];
}

//=================================================================================
bool C_Framebuffer::NeedCheck() const
{
	return m_DirtyFlag;
}

//=================================================================================
void C_Framebuffer::SetChecked()
{
	m_DirtyFlag = false;
}

} // namespace GLEngine::GLRenderer