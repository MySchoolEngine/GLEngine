#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/ModelData.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/GLRenderInterface3D.h>
#include <GLRenderer/GLResourceManager.h>
#include <GLRenderer/OGLRenderer.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

namespace GLEngine::GLRenderer {

//=================================================================================
C_GLRenderInterface3D::C_GLRenderInterface3D()
{
	glGenVertexArrays(1, &m_VAOid);
	const static std::string s_Name("defaultVAO");
	glObjectLabel(GL_VERTEX_ARRAY, m_VAOid, static_cast<GLsizei>(s_Name.length()), s_Name.c_str());
}

//=================================================================================
C_GLRenderInterface3D::~C_GLRenderInterface3D()
{
	glDeleteVertexArrays(1, &m_VAOid);
}

//=================================================================================
void C_GLRenderInterface3D::Render(const Renderer::RenderCall3D& call)
{
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	auto& glRM	   = dynamic_cast<C_OGLRenderer&>(renderer).GetRMGR();

	GLPipeline* pipeline = glRM.GetPipeline(call.PipelineHandle);
	GLE_ASSERT(pipeline, "No pipeline set");
	if (!pipeline)
	{
		return;
	}

	// bind shader, UBOs
	Shaders::C_ShaderManager& shm = Shaders::C_ShaderManager::Instance();
	shm.ActivateShader(shm.GetProgram(pipeline->GetDesc().shader));

	renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&, this, call]() {
			GLPipeline* pipeline = glRM.GetPipeline(call.PipelineHandle);
			GLE_ASSERT(pipeline, "No pipeline set");
			glBindVertexArray(m_VAOid);

			for (int i = 0; i < pipeline->GetDesc().vertexInput.size(); ++i)
			{
				const Renderer::AttributeDescriptor& vertexDesc = pipeline->GetDesc().vertexInput[i];
				auto*								 buffer		= glRM.GetBuffer(call.Buffers[i]);
				buffer->bind();
				switch (vertexDesc.type)
				{
				case Renderer::E_ShaderDataType::Float:
					glVertexAttribPointer(i, T_GLNumComponents_v<float>, T_TypeToGL<float>::value, GL_FALSE, 0, nullptr);
					break;
				case Renderer::E_ShaderDataType::Vec2:
					glVertexAttribPointer(i, T_GLNumComponents_v<glm::vec2>, T_TypeToGL<glm::vec2>::value, GL_FALSE, 0, nullptr);
					break;
				case Renderer::E_ShaderDataType::Vec3:
					glVertexAttribPointer(i, T_GLNumComponents_v<glm::vec3>, T_TypeToGL<glm::vec3>::value, GL_FALSE, 0, nullptr);
					break;
				case Renderer::E_ShaderDataType::Vec4:
					glVertexAttribPointer(i, T_GLNumComponents_v<glm::vec4>, T_TypeToGL<glm::vec4>::value, GL_FALSE, 0, nullptr);
					break;
				default:
					GLE_ERROR("Unknown shader data type");
					break;
				}
				glEnableVertexAttribArray(i);
				buffer->unbind();
			}
			auto modelData = Buffers::C_UniformBuffersManager::Instance().GetBufferByName("modelData");
			if (auto modelDataUbo = std::dynamic_pointer_cast<Buffers::UBO::C_ModelData>(modelData))
			{
				modelDataUbo->SetModelMatrix(call.ModelMatrix);
				modelDataUbo->SetMaterialIndex(call.MaterialIndex);
				modelDataUbo->UploadData(renderer);
				modelDataUbo->Activate(renderer.GetRM(), true);
			}

			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(call.NumPrimities));


			for (int i = 0; i < pipeline->GetDesc().vertexInput.size(); ++i)
			{
				auto* buffer = glRM.GetBuffer(call.Buffers[i]);
				buffer->unbind();
			}
			glBindVertexArray(0);
		},
		"handles"));
}

} // namespace GLEngine::GLRenderer