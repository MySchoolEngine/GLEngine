#include <GLRendererStdafx.h>

#include <GLRenderer/Components/TerrainMesh.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

const int dim = 256;

namespace GLEngine {
namespace GLRenderer {
namespace Components {


//=================================================================================
C_TerrainMesh::C_TerrainMesh()
	: m_Frequency(13)
	, m_SqPerLine(5)
	, m_Noise("TerrainNoise")
{
	m_Terrain = std::make_shared<Mesh::C_TerrainMeshResource>();
	m_Noise.StartGroupOp();
	m_Noise.SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	m_Noise.SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(m_Noise.GetTarget(), 0, GL_RGBA32F, dim, dim, 0, GL_RGBA, GL_FLOAT, nullptr);
	m_Noise.SetDimensions({ dim,dim });
	m_Noise.GenerateMipMaps();
	m_Noise.EndGroupOp();
}

//=================================================================================
void C_TerrainMesh::PerformDraw() const
{

	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					auto& shmgr = Shaders::C_ShaderManager::Instance();
					shmgr.ActivateShader(shmgr.GetProgram("noise"));
					shmgr.GetProgram("noise")->SetUniform("frequency", m_Frequency);
				}
			)
		)
	);
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					glMemoryBarrier(GL_ALL_BARRIER_BITS);
					glActiveTexture(GL_TEXTURE0);
					glBindImageTexture(0, m_Noise.GetTexture(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

					glDispatchCompute(dim / 16, dim / 16, 1);
					glMemoryBarrier(GL_ALL_BARRIER_BITS);
					m_Noise.bind();
					glGenerateMipmap(m_Noise.GetTarget());
					m_Noise.unbind();
				}
			)
		)
	);
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					auto& shmgr = Shaders::C_ShaderManager::Instance();
					auto shader = shmgr.GetProgram("terrain");
					shmgr.ActivateShader(shader);
					shader->SetUniform("tex", 0);
					shader->SetUniform("sqPerLine", static_cast<float>(m_SqPerLine));
					shader->SetUniform("modelMatrix", glm::mat4(1.0f));
					shader->SetUniform("modelColor", glm::vec4(0.3f, 1.0f, 0.4,0.0f));
					shader->SetUniform("hasTexture", false);

					glActiveTexture(GL_TEXTURE0);
					m_Noise.bind();

					m_Terrain->BindVAO();
					glDrawArrays(GL_TRIANGLES, 0, 6*m_SqPerLine*m_SqPerLine);
					m_Terrain->UnbindVAO();
				}
			)
		)
	);
}

}}}