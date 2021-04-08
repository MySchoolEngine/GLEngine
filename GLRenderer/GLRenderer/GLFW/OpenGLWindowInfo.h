#pragma once

#include <Core/CoreEnums.h>
#include <Core/WindowInfo.h>

namespace GLEngine::GLRenderer { namespace GLFW {

//=================================================================================
struct S_OpenGLWindowInfo : public Core::S_WindowInfo {
	S_OpenGLWindowInfo(unsigned int width, unsigned int height)
		: S_WindowInfo(width, height)
	{
	}
	unsigned int m_MajorVersion = 4;
	unsigned int m_MinorVersion = 3;

	virtual Core::E_Driver GetDriver() const override { return Core::E_Driver::OpenGL; }
};

}} // namespace GLEngine::GLRenderer::GLFW