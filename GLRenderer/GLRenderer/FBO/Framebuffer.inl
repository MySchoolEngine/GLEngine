#pragma once

#include <GLRenderer/Commands/FBO/GLBindFramebuffer.h>

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
}