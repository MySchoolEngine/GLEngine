#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/Shaders/GLUseProgram.h>


namespace GLEngine {
namespace GLRenderer {
namespace Commands {

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
std::shared_ptr<GLEngine::Renderer::I_Resource> C_GLUseProgram::GetResource() const
{
	return nullptr;
}

}}}