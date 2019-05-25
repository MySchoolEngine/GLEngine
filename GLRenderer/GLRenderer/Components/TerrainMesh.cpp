#include <GLRendererStdafx.h>

#include <GLRenderer/Components/TerrainMesh.h>

#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Buffers/UBO/RainDataBuffer.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <GLRenderer/Textures/TextureUnitManager.h>

#include <GLRenderer/PersistentDebug.h>
#include <GLRenderer/Debug.h>

#include <Physics/Primitives/AABB.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

const int dim = 1024;
// if you make this mutable you need to update modelMatrix whenever you 
// mutate this value
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
	, m_RainData(std::make_shared<decltype(m_RainData)::element_type>("rainData", 1, dim))
	, m_NumDrops(100)
	, m_HasTexture(false)
	, m_UsePerlin(false)
	, m_QueuedUpdate(false)
	, m_QueueSimulation(false)
	, m_Selected(false)
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

	m_RainData->GenerateDrops();
	m_HasTexture.SetName("Use texture");

	GenerateTerrain();

	CalculateStats();
}

//=================================================================================
void C_TerrainMesh::PerformDraw() const
{
	auto& tm = Textures::C_TextureUnitManger::Instance();
	tm.BindTextureToUnit(m_Noise, 0);

	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					auto& shmgr = Shaders::C_ShaderManager::Instance();
					auto shader = shmgr.GetProgram("terrain");
					shmgr.ActivateShader(shader);
					shader->SetUniform("patchSize", patchSize);
					shader->BindSampler(m_Noise, 0);
					shader->SetUniform("sqPerLine", static_cast<float>(m_SqPerLine));
					shader->SetUniform("modelMatrix", GetModelMatrix());
					shader->SetUniform("modelColor", glm::vec4(0.3f, 1.0f, 0.4, 0.0f));
					shader->SetUniform("hasTexture", m_HasTexture.GetValue());

					m_Terrain->BindVAO();
					glDrawArrays(GL_TRIANGLES, 0, 6*m_SqPerLine*m_SqPerLine);
					m_Terrain->UnbindVAO();
				}
			)
		)
	);

	// rim render
	tm.BindTextureToUnit(m_Noise, 0);

	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					auto& shmgr = Shaders::C_ShaderManager::Instance();
					auto shader = shmgr.GetProgram("terrain-rim");
					shmgr.ActivateShader(shader);
					shader->SetUniform("patchSize", patchSize);
					shader->BindSampler(m_Noise, 0);
					shader->SetUniform("sqPerLine", m_SqPerLine);
					shader->SetUniform("modelMatrix", GetModelMatrix());
					shader->SetUniform("modelColor", glm::vec4(0.3f, 1.0f, 0.4, 0.0f));

					m_Terrain->BindVAO();
					glDrawArrays(GL_TRIANGLES, 0, 6 * m_SqPerLine*4);
					m_Terrain->UnbindVAO();
				}
			)
		)
	);


	if (m_Selected) {
		Core::C_Application::Get().GetActiveRenderer()->AddCommand(
			std::move(
				std::make_unique<Commands::HACK::C_LambdaCommand>(
					[&]() {
						auto& dd = C_DebugDraw::Instance();
						dd.DrawAABB(m_AABB,glm::vec3(1.0f,0.0f,0.0f),GetModelMatrix());
					}
				)
			)
		);
	}


	if (m_QueuedUpdate) {
		CalculateStats();
	}
}

//=================================================================================
void C_TerrainMesh::SetCoord(glm::ivec2 coord)
{
	m_Coord = coord;
	m_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(m_Coord.x*patchSize, 0.0f, m_Coord.y*patchSize));
}

//=================================================================================
void C_TerrainMesh::SetFrequncy(int freq)
{
	if (m_Frequency != freq) {
		m_Frequency = freq;
		GenerateTerrain();
		//m_QueuedUpdate = true;
	}
}

//=================================================================================
void C_TerrainMesh::UsePerlinNoise(bool val)
{
	if (m_UsePerlin != val) {
		m_UsePerlin = val;
		GenerateTerrain();
		//m_QueuedUpdate = true;
	}
}

//=================================================================================
void C_TerrainMesh::UpdateStats()
{
	if (!m_QueuedUpdate) {
		return;
	}
	m_QueuedUpdate = false;

	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					using namespace Physics::Primitives;
					m_Stats.DownloadData();
					m_AABB = S_AABB();
				
					m_AABB.Add(glm::vec3(0.0f, m_Stats.min, 0.0f));
					m_AABB.Add(glm::vec3(patchSize, m_Stats.max, patchSize));
				}
			)
		)
	);
}

//=================================================================================
void C_TerrainMesh::GenerateTerrain()
{
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					auto& shmgr = Shaders::C_ShaderManager::Instance();
					shmgr.ActivateShader(shmgr.GetProgram("noise"));
					shmgr.GetProgram("noise")->SetUniform("frequency", m_Frequency);
					shmgr.GetProgram("noise")->SetUniform("unicoord", (m_Coord*(dim-1)));
					shmgr.GetProgram("noise")->SetUniform("patchWidth", dim);
					shmgr.GetProgram("noise")->SetUniform("usePerlin", m_UsePerlin);
				}
			)
		)
	);

	auto& tm = Textures::C_TextureUnitManger::Instance();
	tm.BindImageToUnit(m_Noise, 0, Textures::E_Access::Write);

	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					RenderDoc::C_DebugScope s("NoiseCompute");

					glDispatchCompute(dim / 16, dim / 16, 1);
					glMemoryBarrier(GL_ALL_BARRIER_BITS);

					m_Noise.bind();
					glGenerateMipmap(m_Noise.GetTarget());
					m_Noise.unbind();
				}
			)
		)
	);
}

//=================================================================================
void C_TerrainMesh::CalculateStats() const
{

	auto& tm = Textures::C_TextureUnitManger::Instance();
	tm.BindImageToUnit(m_Noise, 0, Textures::E_Access::Read);

	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					RenderDoc::C_DebugScope s("Terrain stats");
					auto& shmgr = Shaders::C_ShaderManager::Instance();
					shmgr.ActivateShader(shmgr.GetProgram("stats"));
					m_Stats.bind();

					glDispatchCompute(1, 1, 1);
					glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
					m_Stats.unbind();
				}
			)
		)
	);
}

//=================================================================================
void C_TerrainMesh::Simulate()
{
	auto& tm = Textures::C_TextureUnitManger::Instance();
	tm.BindImageToUnit(m_Noise, 0, Textures::E_Access::ReadWrite);

	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					RenderDoc::C_DebugScope s("Terrain erosion");
					auto& shmgr = Shaders::C_ShaderManager::Instance();
					auto program = shmgr.GetProgram("erosion");
					if (!program) return;
					shmgr.ActivateShader(program);
					program->SetUniform("numDrops", m_NumDrops);
					program->SetUniform("numSteps", 150);
					program->SetUniform("inertia", 0.025f);
					m_RainData->UploadData();
					m_RainData->Activate(true);

					glDispatchCompute(1, 1, 1);
					glMemoryBarrier(GL_ALL_BARRIER_BITS);
					m_Noise.GenerateMipMaps();
					m_RainData->Activate(false);
					// generate new droplets
					m_RainData->GenerateDrops();
				}
			)
		)
	);
}

//=================================================================================
void C_TerrainMesh::DebugDraw()
{
	auto& debug = C_DebugDraw::Instance();
	float OnePixel = patchSize / dim;
	for (int i = 0; i < m_NumDrops; ++i) {
		auto dropCoord = m_RainData->m_RainDrops[i];
		auto dropPoint = glm::vec4(dropCoord.x*OnePixel, 2, dropCoord.y*OnePixel, 1.0);
		auto fallPoint = glm::vec4(dropCoord.x*OnePixel, -2, dropCoord.y*OnePixel, 1.0);
		const auto modelMatrix = GetModelMatrix();
		const auto dropPointInModelSpace = modelMatrix * dropPoint;

		debug.DrawPoint(dropPointInModelSpace, glm::vec3(0,0,1));
		debug.DrawLine(dropPointInModelSpace, modelMatrix *(fallPoint), glm::vec3(0, 0, 1));
	}

	if (m_Selected) {
		::ImGui::Begin("Terrain ", &m_Selected);
			::ImGui::Image((void*)GetTexture().GetTexture(),
			{
				256,
				256
			},
			{ 0,1 }, { 1,0 });
			m_HasTexture.Draw();
		::ImGui::End();
	}
}

//=================================================================================
void C_TerrainMesh::OnEvent(Core::I_Event& event)
{
	event.m_Handeld = true;
	m_Selected = !m_Selected;
	m_QueuedUpdate = m_Selected;
}

//=================================================================================
glm::mat4 C_TerrainMesh::GetModelMatrix() const
{
	return m_ModelMatrix;
}

}}}