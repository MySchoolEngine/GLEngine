#include "GLW/Framebuffer.h"
#include "GLW/Texture.h"

#include "Debug/Debug.h"

#include <iostream>

namespace GLW {


	//=================================================================================
	C_Framebuffer::C_Framebuffer()
	{
		glGenFramebuffers(1, &m_FBO);

	}

	//=================================================================================
	C_Framebuffer::~C_Framebuffer()
	{
		DestructorFullCheck();
	}

	//=================================================================================
	void C_Framebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
		ErrorCheck();
	}

	//=================================================================================
	void C_Framebuffer::Unbind()
	{
		ErrorCheck();
		// switch back to window-system-provided framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		ErrorCheck();
	}

	//=================================================================================
	void C_Framebuffer::AttachTexture(GLenum attachement, std::shared_ptr<C_Texture> texture)
	{
		if (m_attachements.find(attachement) != m_attachements.end()) {
			std::cerr << "Attaching multiple textures to one attach point in framebuffer" << std::endl;
		}
		Bind();
		glFramebufferTexture(GL_FRAMEBUFFER, attachement, texture->GetTexture(), 0);
		Unbind();

		ErrorCheck();
		m_attachements[attachement] = texture;
	}

	//=================================================================================
	std::shared_ptr<GLW::C_Texture> C_Framebuffer::GetAttachement(GLenum attachement)
	{
		assert(m_attachements.find(attachement) != m_attachements.end());
		return m_attachements[attachement];
	}

}