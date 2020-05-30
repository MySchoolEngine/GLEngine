#include <GLRendererStdAfx.h>

#include <GLRenderer/Commands/Textures/GLMakeTextureHandleResident.h>

namespace GLEngine::GLRenderer::Commands {

//=================================================================================
C_GLMakeTextureHandleResident::C_GLMakeTextureHandleResident(std::uint64_t handle, bool makeResident)
	: m_Handle(handle)
	, m_MakeResident(makeResident)
{

}

//=================================================================================
void C_GLMakeTextureHandleResident::Commit()
{
	if (m_MakeResident)
		glMakeTextureHandleResidentARB(m_Handle);
	else
		glMakeTextureHandleNonResidentARB(m_Handle);
}

}
