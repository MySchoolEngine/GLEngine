#include <GLRendererStdafx.h>

#include <GLRenderer/FBO/Framebuffer.h>
#include <GLRenderer/Textures/Texture.h>

#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>

#include <Core/Application.h>

namespace GLEngine::GLRenderer {

//=================================================================================
C_Framebuffer::C_Framebuffer(const std::string_view name, const bool defaultRendertarget)
	: m_DirtyFlag(true)
	, m_FBO(0)
{
	if (!defaultRendertarget)
	{
		glGenFramebuffers(1, &m_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
		glObjectLabel(GL_FRAMEBUFFER, m_FBO, static_cast<GLsizei>(name.size()), name.data());
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

//=================================================================================
C_Framebuffer::~C_Framebuffer()
{
	if (IsDefaultRenderTarget())
		return;
	auto& device = Core::C_Application::Get().GetActiveRenderer().GetDevice();
	for (auto& [ID, Attachment] : m_Attachments)
	{
		device.DestroyTexture(*(Attachment.get()));
	}
	glDeleteBuffers(1, &m_FBO);
}

//=================================================================================
std::shared_ptr<Textures::C_Texture> C_Framebuffer::GetAttachment(GLenum attachments)
{
	GLE_ASSERT(!IsDefaultRenderTarget(), "Cant GetAttachment of the default render-target.");
	GLE_ASSERT(m_Attachments.find(attachments) != m_Attachments.end(), "There is no attachment of type {}", attachments);
	return m_Attachments[attachments];
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