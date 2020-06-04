/**
 * ==============================================
 * \file 		Framebuffer.h
 * \date 		2018/03/17 19:07
 * \project 	Computer Graphics Project
 * \faculty 	Faculty of Information Technology 
 * \university 	Brno University of Technology
 *
 * \author Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * ==============================================
 */

#pragma once

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

namespace GLEngine::GLRenderer {

namespace Textures {
class C_Texture;
}

class C_Framebuffer {
public:
	explicit C_Framebuffer(const std::string& name);
	~C_Framebuffer();

	template<E_FramebufferTarget target = E_FramebufferTarget::Framebuffer>
	void Bind();
	template<E_FramebufferTarget target = E_FramebufferTarget::Framebuffer>
	void Unbind();

	template<E_FramebufferTarget target = E_FramebufferTarget::Framebuffer>
	void AttachTexture(GLenum attachement, std::shared_ptr<Textures::C_Texture> texture);
	[[nodiscard]] std::shared_ptr<Textures::C_Texture> GetAttachement(GLenum attachement);

	template<E_FramebufferTarget target = E_FramebufferTarget::Framebuffer>
  [[nodiscard]] std::future<bool> CheckCompleteness() const;

	[[nodiscard]] bool NeedCheck() const;
	void SetChecked();

private:
	GLuint	m_FBO;
	bool		m_DirtyFlag;
	std::map<GLenum, std::shared_ptr<Textures::C_Texture>> m_attachements;
};

}

#include <GLRenderer/FBO/Framebuffer.inl>