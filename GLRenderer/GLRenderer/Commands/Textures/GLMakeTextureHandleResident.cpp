#include <GLRendererStdafx.h>

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

//=================================================================================
std::string C_GLMakeTextureHandleResident::GetDescriptor() const
{
	return fmt::format("GLMakeTextureHandleResident handle: {} makeResident: {}", m_Handle, m_MakeResident);
}

} // namespace GLEngine::GLRenderer::Commands
