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
	, m_Noise("TerrainNoise")
{
	m_Terrain = std::make_shared<Mesh::C_TerrainMeshResource>();
	m_Noise.StartGroupOp();
	m_Noise.SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	m_Noise.SetFilter(GL_NEAREST, GL_NEAREST);
	glTexImage2D(m_Noise.GetTarget(), 0, GL_RGBA32F, dim, dim, 0, GL_RGBA, GL_FLOAT, nullptr);
	m_Noise.SetDimensions({ dim,dim });
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
					}
				)
		)
	);
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
					[&]() {
						auto& shmgr = Shaders::C_ShaderManager::Instance();
						shmgr.ActivateShader(shmgr.GetProgram("terrain"));

						m_Terrain->BindVAO();
						glDrawArrays(GL_TRIANGLES, 0, 16);
						m_Terrain->UnbindVAO();
					}
				)
		)
	);
}

}}}