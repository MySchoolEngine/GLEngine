#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/GlClearColor.h>


namespace GLEngine::GLRenderer::Commands {

//=================================================================================
C_GLClearColor::C_GLClearColor(glm::vec4&& color)
	: m_Colour(std::move(color))
{}

//=================================================================================
C_GLClearColor::C_GLClearColor(glm::vec3 color)
	: C_GLClearColor(glm::vec4(color, 0.0f))
{}

//=================================================================================
void C_GLClearColor::Commit()
{
	glClearColor(m_Colour.r, m_Colour.g, m_Colour.b, m_Colour.a);
}

//=================================================================================
GLEngine::Renderer::I_RenderCommand::E_Type C_GLClearColor::GetType() const
{
	return Renderer::I_RenderCommand::E_Type::StateChange;
}

//=================================================================================
std::shared_ptr<GLEngine::Renderer::I_Resource> C_GLClearColor::GetResource() const
{
	return nullptr;
}

//=================================================================================
std::string C_GLClearColor::GetDescriptor() const
{
	return fmt::format("GLClearColor {}", m_Colour);
}

}