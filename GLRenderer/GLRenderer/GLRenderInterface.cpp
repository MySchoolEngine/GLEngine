#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/GLRenderInterface.h>
#include <GLRenderer/GLResourceManager.h>
#include <GLRenderer/OGLRenderer.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

namespace GLEngine::GLRenderer {

//=================================================================================
C_GLRenderInterface::C_GLRenderInterface()
{
	glGenVertexArrays(1, &m_VAOid);
	const static std::string name("defaultVAO");
	glObjectLabel(GL_VERTEX_ARRAY, m_VAOid, static_cast<GLsizei>(name.length()), name.c_str());
}

//=================================================================================
C_GLRenderInterface::~C_GLRenderInterface()
{
	glDeleteVertexArrays(1, &m_VAOid);
}

//=================================================================================
void C_GLRenderInterface::Render(const Renderer::RenderCall3D& call)
{
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	auto& glRM	   = static_cast<C_OGLRenderer&>(renderer).GetRMGL();

	// bind shader, UBOs
	Shaders::C_ShaderManager& shm = Shaders::C_ShaderManager::Instance();
	shm.ActivateShader(shm.GetProgram(call.Shader));

	renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&]() {
			glBindVertexArray(m_VAOid);

			auto* PositionsBuffer = glRM.GetBuffer(call.Buffers[0]);
			PositionsBuffer->bind();
			// read those from pipeline!
			glVertexAttribPointer(0, 4, T_TypeToGL<glm::vec4>::value, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(0);

			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(call.NumPrimities));


			PositionsBuffer->unbind();

			glBindVertexArray(0);
		},
		"handles"));
}

} // namespace GLEngine::GLRenderer
