#pragma once

#include <GLRenderer/Commands/FBO/GLBindFramebuffer.h>
#include <GLRenderer/Commands/FBO/glFramebufferTexture.h>
#include <GLRenderer/Commands/FBO/GLCheckFramebufferStatus.h>

namespace GLEngine::GLRenderer {
//=================================================================================
template<E_FramebufferTarget target /*= E_FramebufferTarget::Framebuffer*/>
void C_Framebuffer::Bind()
{
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::C_GLBindFramebuffer<target>>(m_FBO)
		)
	);
}
//=================================================================================
template<E_FramebufferTarget target /*= E_FramebufferTarget::Framebuffer*/>
void C_Framebuffer::Unbind()
{
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::C_GLBindFramebuffer<target>>(0)
		)
	);
}

//=================================================================================
template<E_FramebufferTarget target /*= E_FramebufferTarget::Framebuffer*/>
void C_Framebuffer::AttachTexture(GLenum attachement, std::shared_ptr<Textures::C_Texture> texture)
{
	const auto it = m_attachements.find(attachement);
	if (it != m_attachements.end())
	{
		if (it->second == texture)
		{
			return;
		}
	}
	Bind<target>();
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::C_glFramebufferTexture<target>>(attachement, texture)
		)
	);
	Unbind<target>();

	m_attachements[attachement] = texture;
	m_DirtyFlag = true;
}

//=================================================================================
template<E_FramebufferTarget target /*= E_FramebufferTarget::Framebuffer*/>
std::future<bool> GLEngine::GLRenderer::C_Framebuffer::CheckCompleteness() const
{
	std::promise<bool> retPromise;
	auto ret = retPromise.get_future();


	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::C_GLCheckFramebufferStatus<target>>(std::move(retPromise))
		)
	);
	return ret;
}
}