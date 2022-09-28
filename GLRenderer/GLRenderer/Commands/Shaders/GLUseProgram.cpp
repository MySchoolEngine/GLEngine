#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/Shaders/GLUseProgram.h>


namespace GLEngine::GLRenderer::Commands {

//=================================================================================
C_GLUseProgram::C_GLUseProgram(GLuint program)
	: m_Proram(program)
{
}

//=================================================================================
void C_GLUseProgram::Commit()
{
	glUseProgram(m_Proram);
}

//=================================================================================
Renderer::I_RenderCommand::E_Type C_GLUseProgram::GetType() const
{
	return Renderer::I_RenderCommand::E_Type::StateChange;
}

//=================================================================================
std::string C_GLUseProgram::GetDescriptor() const
{
	return fmt::format("UseProgram {}", m_Proram);
}

} // namespace GLEngine::GLRenderer::Commands