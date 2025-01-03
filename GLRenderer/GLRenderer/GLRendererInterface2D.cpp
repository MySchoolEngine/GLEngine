#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/GLRendererInterface2D.h>
#include <GLRenderer/Helpers/RenderHelpers.h>
#include <GLRenderer/OGLRenderer.h>
#include <GLRenderer/Shaders/ShaderManager.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Mesh/Geometry.h>
#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/Resources/ResourceManager.h>

#include <Core/Application.h>
#include <Core/Resources/ResourceManager.h>


namespace GLEngine::GLRenderer {

//=================================================================================
C_GLRendererInterface2D::C_GLRendererInterface2D(const glm::uvec2 size)
	: m_RenderTargetSize(size)
{
	glGenVertexArrays(1, &m_VAOid);
	const static std::string s_Name("defaultVAO2D");
	glObjectLabel(GL_VERTEX_ARRAY, m_VAOid, static_cast<GLsizei>(s_Name.length()), s_Name.c_str());

	Renderer::I_Renderer&	   renderer = Core::C_Application::Get().GetActiveRenderer();
	Renderer::ResourceManager& rrm		= renderer.GetRM();

	{
		const auto mesh = Renderer::MeshData::C_Geometry::CreatePlane(1, 1);

		const auto positionsSize = static_cast<uint32_t>(sizeof(mesh.vertices[0]) * mesh.vertices.size());
		m_PlaneVertHandle		 = rrm.createBuffer(Renderer::BufferDescriptor{
				   .size  = positionsSize,
				   .type  = Renderer::E_BufferType::Vertex,
				   .usage = Renderer::E_ResourceUsage::Immutable,
		   });
		renderer.SetBufferData(m_PlaneVertHandle, positionsSize, mesh.vertices.data());

		const auto UVSize = static_cast<uint32_t>(sizeof(mesh.texcoords[0]) * mesh.texcoords.size());
		m_UVVertHandle	  = rrm.createBuffer(Renderer::BufferDescriptor{
			   .size  = UVSize,
			   .type  = Renderer::E_BufferType::Vertex,
			   .usage = Renderer::E_ResourceUsage::Immutable,
		   });
		renderer.SetBufferData(m_UVVertHandle, UVSize, mesh.texcoords.data());
	}


	auto& glRM = dynamic_cast<C_OGLRenderer&>(renderer).GetRMGR();
	glBindVertexArray(m_VAOid);
	{
		auto* buffer = glRM.GetBuffer(m_PlaneVertHandle);
		buffer->bind();
		glVertexAttribPointer(0, T_GLNumComponents_v<glm::vec3>, T_TypeToGL<glm::vec3>::value, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(0);
	}
	{
		auto* buffer = glRM.GetBuffer(m_UVVertHandle);
		buffer->bind();
		glVertexAttribPointer(1, T_GLNumComponents_v<glm::vec2>, T_TypeToGL<glm::vec2>::value, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(1);
	}
	glBindVertexArray(0);
}

//=================================================================================
C_GLRendererInterface2D::~C_GLRendererInterface2D()
{
	// delete handle
	glDeleteVertexArrays(1, &m_VAOid);
	// Renderer::I_Renderer& renderer = Core::C_Application::Get().GetActiveRenderer();
	// Renderer::ResourceManager& rm = renderer.GetRM();
	//
	// rm.destoryBuffer();
}

//=================================================================================
void C_GLRendererInterface2D::Render(const Renderer::RenderCall2D& call)
{
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	auto& glRM	   = dynamic_cast<C_OGLRenderer&>(renderer).GetRMGR();

	GLPipeline* pipeline = glRM.GetPipeline(call.PipelineHandle);
	GLE_ASSERT(pipeline, "No pipeline set");
	if (!pipeline)
	{
		return;
	}
	Shaders::C_ShaderManager& shm			= Shaders::C_ShaderManager::Instance();
	auto					  shaderProgram = shm.GetProgram(pipeline->GetDesc().shader);
	shm.ActivateShader(shaderProgram);
	renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&, shaderProgram, pipeline]() {
			if (pipeline->GetDesc().blending.empty() == false)
				SetBlendFunction(0, pipeline->GetDesc().blending[0]);

			glm::mat4 projection = glm::ortho(0.0f, m_RenderTargetSize.x, m_RenderTargetSize.y, 0.0f, -1.0f, 1.0f);
			shaderProgram->SetUniform("ProjectionMatrix", projection);
			shaderProgram->SetUniform("Size", call.Size);
			shaderProgram->SetUniform("Position", glm::vec2(call.Position));
			glDisable(GL_DEPTH_TEST);
			glBindVertexArray(m_VAOid);
			glViewport(0, 0, m_RenderTargetSize.x, m_RenderTargetSize.y);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
		},
		"2D Handles"));
}
} // namespace GLEngine::GLRenderer