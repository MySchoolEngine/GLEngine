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

#include <Renderer/Resources/RenderResourceHandle.h>

namespace GLEngine::GLRenderer {

class C_Framebuffer {
public:
	~C_Framebuffer();

	template <E_FramebufferTarget Target = E_FramebufferTarget::Framebuffer> void Bind();
	template <E_FramebufferTarget Target = E_FramebufferTarget::Framebuffer> void Unbind();

	template <E_FramebufferTarget Target = E_FramebufferTarget::Framebuffer> void AttachTexture(GLenum attachment, Renderer::Handle<Renderer::Texture> texture);
	[[nodiscard]] Renderer::Handle<Renderer::Texture>							  GetAttachment(GLenum attachments);

	template <E_FramebufferTarget Target = E_FramebufferTarget::Framebuffer> [[nodiscard]] std::future<bool> CheckCompleteness() const;

	[[nodiscard]] bool NeedCheck() const;
	void			   SetChecked();

	bool IsDefaultRenderTarget() const { return m_FBO == 0; }

private:
	explicit C_Framebuffer(const std::string_view name, bool defaultRenderTarget = false);
	GLuint												  m_FBO;
	bool												  m_DirtyFlag;
	std::map<GLenum, Renderer::Handle<Renderer::Texture>> m_Attachments;

	friend class C_GLDevice;
};


} // namespace GLEngine::GLRenderer

#include <GLRenderer/FBO/Framebuffer.inl>