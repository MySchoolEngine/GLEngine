#include <GLRendererStdafx.h>

#include <GLRenderer/Components/TerrainMesh.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <Physics/Primitives/AABB.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

const int dim = 1024;
const static float patchSize = 8;

namespace GLEngine {
namespace GLRenderer {
namespace Components {


//=================================================================================
C_TerrainMesh::C_TerrainMesh()
	: m_Frequency(13)
	, m_SqPerLine(5)
	, m_Noise("TerrainNoise")
	, m_Coord(0, 0)
	, m_Stats(3)
	, m_HasTexture(false)
	, m_UsePerlin(false)
	, m_QueuedUpdate(false)
{
	m_Terrain = std::make_shared<Mesh::C_TerrainMeshResource>();
	m_Noise.StartGroupOp();
	m_Noise.SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	m_Noise.SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(m_Noise.GetTarget(), 0, GL_RGBA32F, dim, dim, 0, GL_RGBA, GL_FLOAT, nullptr);
	m_Noise.SetDimensions({ dim,dim });
	m_Noise.GenerateMipMaps();
	m_Noise.EndGroupOp();

	m_AABB.Add(glm::vec3(0.0f, 0.0f, 0.0f));
	m_AABB.Add(glm::vec3(patchSize, 1.0f, patchSize));
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
					shmgr.GetProgram("noise")->SetUniform("unicoord", m_Coord*dim);
					shmgr.GetProgram("noise")->SetUniform("patchWidth", dim);
					shmgr.GetProgram("noise")->SetUniform("usePerlin", m_UsePerlin);
				}
			)
		)
	);
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					RenderDoc::C_DebugScope s("NoiseCompute");
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
	
	if (m_QueuedUpdate) {
		Core::C_Application::Get().GetActiveRenderer()->AddCommand(
			std::move(
				std::make_unique<Commands::HACK::C_LambdaCommand>(
					[&]() {
						RenderDoc::C_DebugScope s("Terrain stats");
						auto& shmgr = Shaders::C_ShaderManager::Instance();
						shmgr.ActivateShader(shmgr.GetProgram("stats"));
						m_Stats.bind();
						glMemoryBarrier(GL_ALL_BARRIER_BITS);
						glActiveTexture(GL_TEXTURE0);
						glBindImageTexture(0, m_Noise.GetTexture(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

						glDispatchCompute(1, 1, 1);
						glMemoryBarrier(GL_ALL_BARRIER_BITS);
						m_Stats.unbind();
					}
				)
			)
		);
	}


	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					auto& shmgr = Shaders::C_ShaderManager::Instance();
					auto shader = shmgr.GetProgram("terrain");
					shmgr.ActivateShader(shader);
					shader->SetUniform("patchSize", patchSize);
					shader->SetUniform("tex", 0);
					shader->SetUniform("sqPerLine", static_cast<float>(m_SqPerLine));
					shader->SetUniform("modelMatrix", glm::translate(glm::mat4(1.0f), glm::vec3(m_Coord.x*patchSize, 0.0f, m_Coord.y*patchSize)));
					shader->SetUniform("modelColor", glm::vec4(0.3f, 1.0f, 0.4, 0.0f));
					shader->SetUniform("hasTexture", m_HasTexture);

					glActiveTexture(GL_TEXTURE0);
					m_Noise.bind();

					m_Terrain->BindVAO();
					glDrawArrays(GL_TRIANGLES, 0, 6*m_SqPerLine*m_SqPerLine);
					m_Terrain->UnbindVAO();
				}
			)
		)
	);

	if (m_HasTexture) {
		Core::C_Application::Get().GetActiveRenderer()->AddCommand(
			std::move(
				std::make_unique<Commands::HACK::C_LambdaCommand>(
					[&]() {
						auto& dd = C_DebugDraw::Instance();
						dd.DrawAABB(m_AABB,glm::vec3(1.0f,0.0f,0.0f),glm::translate(glm::mat4(1.0f), glm::vec3(m_Coord.x*patchSize, 0.0f, m_Coord.y*patchSize)));
					}
				)
			)
		);
	}
}

//=================================================================================
void C_TerrainMesh::SetCoord(glm::ivec2 coord)
{
	m_Coord = coord;
}

//=================================================================================
void C_TerrainMesh::UpdateStats()
{
	if (!m_QueuedUpdate) {
		return;
	}
	m_QueuedUpdate = false;
	using namespace Physics::Primitives;
	m_Stats.DownloadData();
	m_AABB = S_AABB();

	m_AABB.Add(glm::vec3(0.0f, m_Stats.min, 0.0f));
	m_AABB.Add(glm::vec3(patchSize, m_Stats.max, patchSize));
}

//=================================================================================
void C_TerrainMesh::OnEvent(Core::I_Event& event)
{
	event.m_Handeld = true;
	m_HasTexture = !m_HasTexture;
	m_QueuedUpdate = m_HasTexture;
}

}}}