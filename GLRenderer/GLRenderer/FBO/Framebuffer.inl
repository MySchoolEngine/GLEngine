#pragma once

#include <GLRenderer/Commands/FBO/GLBindFramebuffer.h>
#include <GLRenderer/Commands/FBO/GLCheckFramebufferStatus.h>
#include <GLRenderer/Commands/FBO/glFramebufferTexture.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

namespace GLEngine::GLRenderer {
//=================================================================================
template <E_FramebufferTarget Target /*= E_FramebufferTarget::Framebuffer*/> void C_Framebuffer::Bind()
{
	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::C_GLBindFramebuffer<Target>>(m_FBO));
}
//=================================================================================
template <E_FramebufferTarget Target /*= E_FramebufferTarget::Framebuffer*/> void C_Framebuffer::Unbind()
{
	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::C_GLBindFramebuffer<Target>>(0));
}

//=================================================================================
template <E_FramebufferTarget Target /*= E_FramebufferTarget::Framebuffer*/> void C_Framebuffer::AttachTexture(GLenum attachment, Renderer::Handle<Renderer::Texture> texture)
{
	GLE_ASSERT(!IsDefaultRenderTarget(), "Cant attach to default render-target.");
	const auto it = m_Attachments.find(attachment);
	if (it != m_Attachments.end())
	{
		if (it->second == texture)
		{
			return;
		}
	}
	Bind<Target>();
	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::C_glFramebufferTexture<Target>>(attachment, texture));
	Unbind<Target>();

	m_Attachments[attachment] = texture;
	m_DirtyFlag					= true;
}

//=================================================================================
template <E_FramebufferTarget target /*= E_FramebufferTarget::Framebuffer*/> std::future<bool> GLEngine::GLRenderer::C_Framebuffer::CheckCompleteness() const
{
	GLE_ASSERT(!IsDefaultRenderTarget(), "Cant CheckCompleteness to default render-target.");
	std::promise<bool> retPromise;
	auto			   ret = retPromise.get_future();

	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::C_GLCheckFramebufferStatus<target>>(std::move(retPromise)));
	return ret;
}
} // namespace GLEngine::GLRenderer