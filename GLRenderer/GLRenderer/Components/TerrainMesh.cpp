#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/RainDataBuffer.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Components/TerrainMesh.h>
#include <GLRenderer/Debug.h>
#include <GLRenderer/PersistentDebug.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/Textures/TextureUnitManager.h>

#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>

#include <Physics/Primitives/AABB.h>

#include <Core/Application.h>

const int dim = 1024;
// if you make this mutable you need to update modelMatrix whenever you
// mutate this value


// 0 layer - height
// 1 layer - amount of water
// 2 layer - lowest layer - rock
// 3 layer - second layer - sand
// 4 layer - third layer - dirt
const int s_numSedimentLayer = 3;

namespace GLEngine::GLRenderer::Components {


//=================================================================================
C_TerrainMesh::C_TerrainMesh(C_TerrainEntity::S_TerrainSettings* settings)
	: Renderer::I_RenderableComponent(nullptr)
	, m_Noise(
			Renderer::TextureDescriptor{
				"TerrainNoise", 
				dim, dim, 
				Renderer::E_TextureType::TEXTURE_2D_ARRAY, 
				Renderer::E_TextureFormat::R32f,			// could be R16f but TODO find why it doesn't work
				false, 
				1, 1, 
				2 + s_numSedimentLayer
			}
		)
	, m_Coord(0, 0)
	, m_Stats(3)
	, m_RainData(std::make_shared<decltype(m_RainData)::element_type>("rainData", 4, dim))
	, m_HasTexture(false, "Use texture")
	, m_QueuedUpdate(false)
	, m_QueueSimulation(false)
	, m_Selected(false)
{
	auto& device = Core::C_Application::Get().GetActiveRenderer().GetDevice();

	device.AllocateTexture(m_Noise);
	ErrorCheck();
	m_Noise.SetFilter(Renderer::E_TextureFilter::Linear, Renderer::E_TextureFilter::Linear);

	m_Terrain = std::make_shared<Mesh::C_TerrainMeshResource>();

	SetSettings(settings);

	m_RainData->GenerateDrops();

	GenerateTerrain();
}

//=================================================================================
C_TerrainMesh::C_TerrainMesh(Textures::C_Texture&& texture)
	: Renderer::I_RenderableComponent(nullptr)
	, m_Noise(std::move(texture))
	, m_Coord(0, 0)
	, m_Stats(3)
	, m_RainData(std::make_shared<decltype(m_RainData)::element_type>("rainData", 1, dim))
	, m_HasTexture(false, "Use texture")
	, m_QueuedUpdate(false)
	, m_QueueSimulation(false)
	, m_Selected(false)
	, m_Settings(nullptr)
{
	m_Terrain = std::make_shared<Mesh::C_TerrainMeshResource>();

	m_RainData->GenerateDrops();
}

//=================================================================================
C_TerrainMesh::~C_TerrainMesh()
{
	auto& device = Core::C_Application::Get().GetActiveRenderer().GetDevice();
	device.DestroyTexture(m_Noise);
}

//=================================================================================
void C_TerrainMesh::SetSettings(C_TerrainEntity::S_TerrainSettings* settings)
{
	m_Settings = settings;

	m_AABB.Add(glm::vec3(0.0f, 0.0f, 0.0f));
	m_AABB.Add(glm::vec3(m_Settings->m_PatchSize, 1.0f, m_Settings->m_PatchSize));
}

//=================================================================================
void C_TerrainMesh::PerformDraw() const
{
	auto& tm = Textures::C_TextureUnitManger::Instance();
	tm.BindTextureToUnit(m_Noise, 0);


	auto& shmgr	 = Shaders::C_ShaderManager::Instance();
	auto  shader = shmgr.GetProgram("terrain");
	shmgr.ActivateShader(shader);

	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[this, shader]() {
			shader->SetUniform("patchSize", static_cast<float>(m_Settings->m_PatchSize));
			shader->SetUniform("tex", 0);
			shader->SetUniform("sqPerLine", static_cast<float>(m_Settings->m_SqPerLine));
			shader->SetUniform("modelMatrix", GetModelMatrix());
			shader->SetUniform("modelColor[0]", m_Settings->m_Layers[0].m_TerrainColor.GetValue());
			shader->SetUniform("modelColor[1]", m_Settings->m_Layers[1].m_TerrainColor.GetValue());
			shader->SetUniform("modelColor[2]", m_Settings->m_Layers[2].m_TerrainColor.GetValue());
			// shader->SetUniform("hasTexture", m_HasTexture.GetValue());
			shader->SetUniform("selected", m_Selected);

			m_Terrain->BindVAO();
			glDrawArrays(GL_TRIANGLES, 0, 6 * m_Settings->m_SqPerLine * m_Settings->m_SqPerLine);
			m_Terrain->UnbindVAO();
		},
		"Terrain - draw"));

	// rim render
	tm.BindTextureToUnit(m_Noise, 0);

	auto ShaderTerrainRim = shmgr.GetProgram("terrain-rim");
	shmgr.ActivateShader(ShaderTerrainRim);

	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[this, ShaderTerrainRim]() {
			ShaderTerrainRim->SetUniform("patchSize", static_cast<float>(m_Settings->m_PatchSize));
			ShaderTerrainRim->SetUniform("tex", 0);
			ShaderTerrainRim->SetUniform("sqPerLine", static_cast<int>(m_Settings->m_SqPerLine));
			ShaderTerrainRim->SetUniform("modelMatrix", GetModelMatrix());
			ShaderTerrainRim->SetUniform("modelColor[0]", m_Settings->m_Layers[0].m_TerrainColor.GetValue());
			ShaderTerrainRim->SetUniform("modelColor[1]", m_Settings->m_Layers[1].m_TerrainColor.GetValue());
			ShaderTerrainRim->SetUniform("modelColor[2]", m_Settings->m_Layers[2].m_TerrainColor.GetValue());

			m_Terrain->BindVAO();
			glDrawArrays(GL_TRIANGLES, 0, 6 * m_Settings->m_SqPerLine * 4);
			m_Terrain->UnbindVAO();
		},
		"Terrain - terrain-rim"));


	if (m_Selected)
	{
		auto& dd = C_DebugDraw::Instance();
		dd.DrawAABB(m_AABB, glm::vec3(1.0f, 0.0f, 0.0f), GetModelMatrix());
	}
}

//=================================================================================
void C_TerrainMesh::SetCoord(glm::ivec2 coord)
{
	m_Coord = coord;
	m_ModelMatrix
		= glm::translate(glm::mat4(1.0f), glm::vec3(m_Coord.x * static_cast<float>(m_Settings->m_PatchSize), 0.0f, m_Coord.y * static_cast<float>(m_Settings->m_PatchSize)));
}

//=================================================================================
void C_TerrainMesh::UpdateStats()
{
	if (!m_QueuedUpdate)
	{
		return;
	}
	m_QueuedUpdate = false;

	RenderDoc::C_DebugScope s("Terrain stats");
	auto&					tm = Textures::C_TextureUnitManger::Instance();
	tm.BindImageToUnit(m_Noise, 0, E_OpenGLAccess::Read);


	auto& shmgr = Shaders::C_ShaderManager::Instance();
	shmgr.ActivateShader(shmgr.GetProgram("stats"));

	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&]() {
			m_Stats.bind();

			glDispatchCompute(1, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			m_Stats.unbind();
			using namespace Physics::Primitives;
			m_Stats.DownloadData();
			m_AABB = S_AABB();

			m_AABB.Add(glm::vec3(0.0f, m_Stats.min, 0.0f));
			m_AABB.Add(glm::vec3(static_cast<float>(m_Settings->m_PatchSize), m_Stats.max, static_cast<float>(m_Settings->m_PatchSize)));
		},
		"Download stats"));
}

//=================================================================================
void C_TerrainMesh::GenerateTerrain()
{
	auto& shmgr		  = Shaders::C_ShaderManager::Instance();
	auto  noiseShader = shmgr.GetProgram("noise");
	shmgr.ActivateShader(noiseShader);

	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[this, noiseShader]() {
			noiseShader->SetUniform("frequency", static_cast<int>(m_Settings->m_Freq));
			noiseShader->SetUniform("unicoord", (m_Coord * (dim - 1)));
			noiseShader->SetUniform("patchWidth", dim);
			noiseShader->SetUniform("usePerlin", static_cast<bool>(m_Settings->PerlinNoise));
			noiseShader->SetUniform("layerWeight[0]", m_Settings->m_Layers[0].m_Weight.GetValue());
			noiseShader->SetUniform("layerWeight[1]", m_Settings->m_Layers[1].m_Weight.GetValue());
			noiseShader->SetUniform("layerWeight[2]", m_Settings->m_Layers[2].m_Weight.GetValue());
		},
		"Prepare generation of noise"));


	RenderDoc::C_DebugScope s("NoiseCompute");
	auto&					tm = Textures::C_TextureUnitManger::Instance();
	tm.BindImageToUnit(m_Noise, 0, E_OpenGLAccess::Write);

	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&]() {
			glDispatchCompute(dim / 16, dim / 16, 1);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);

			m_Noise.GenerateMipMaps();
		},
		"Dispatch noise"));

	m_QueuedUpdate = true;
}

//=================================================================================
void C_TerrainMesh::Simulate()
{
	RenderDoc::C_DebugScope s("Terrain erosion");
	auto&					tm = Textures::C_TextureUnitManger::Instance();
	tm.BindImageToUnit(m_Noise, 0, E_OpenGLAccess::ReadWrite);

	auto& shmgr	  = Shaders::C_ShaderManager::Instance();
	auto  program = shmgr.GetProgram("erosion");
	if (!program)
		return;
	shmgr.ActivateShader(program);

	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[this, program]() {
			program->SetUniform("numDrops", static_cast<int>(m_Settings->m_Drops));
			program->SetUniform("numSteps", static_cast<int>(m_Settings->m_NumSteps));
			program->SetUniform("inertia", m_Settings->m_Inertia);
			program->SetUniform("gravityForce", static_cast<float>(m_Settings->m_Gravitation));
			program->SetUniform("evaporate", static_cast<float>(m_Settings->m_Evaporation));
			program->SetUniform("startingSpeed", static_cast<float>(m_Settings->m_InitWater));
			program->SetUniform("initialWater", static_cast<float>(m_Settings->m_StartingSpeed));
			m_RainData->UploadData();
			m_RainData->Activate(true);

			glDispatchCompute(1, 1, 1);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
			m_RainData->Activate(false);
			// generate new droplets
			m_RainData->GenerateDrops();
		},
		"Simulate"));

	s = RenderDoc::C_DebugScope("collectHeightMap");

	auto noiseShader = shmgr.GetProgram("collectHeightMap");
	shmgr.ActivateShader(noiseShader);


	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&]() {
			glDispatchCompute(dim / 16, dim / 16, 1);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);

			m_Noise.GenerateMipMaps();
		},
		"collectHeightMap"));
}

//=================================================================================
void C_TerrainMesh::DebugDraw()
{
	auto& debug	   = C_DebugDraw::Instance();
	float OnePixel = static_cast<float>(m_Settings->m_PatchSize) / dim;
	for (int i = 0; i < m_Settings->m_Drops; ++i)
	{
		auto	   dropCoord			 = m_RainData->m_RainDrops[i];
		auto	   dropPoint			 = glm::vec4(dropCoord.x * OnePixel, 2, dropCoord.y * OnePixel, 1.0);
		auto	   fallPoint			 = glm::vec4(dropCoord.x * OnePixel, -2, dropCoord.y * OnePixel, 1.0);
		const auto modelMatrix			 = GetModelMatrix();
		const auto dropPointInModelSpace = modelMatrix * dropPoint;

		debug.DrawPoint(dropPointInModelSpace, Colours::blue);
		debug.DrawLine(dropPointInModelSpace, modelMatrix * (fallPoint), Colours::blue);
	}

	if (m_Selected)
	{
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
std::string_view C_TerrainMesh::GetDebugComponentName() const
{
	return "Terrain";
}

//=================================================================================
bool C_TerrainMesh::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
void C_TerrainMesh::OnEvent(Core::I_Event& event)
{
	event.m_Handeld = true;
	m_Selected		= !m_Selected;
	m_QueuedUpdate	= m_Selected;
}

//=================================================================================
Physics::Primitives::S_AABB C_TerrainMesh::GetAABB() const
{
	return m_AABB;
}

//=================================================================================
glm::mat4 C_TerrainMesh::GetModelMatrix() const
{
	return m_ModelMatrix;
}

} // namespace GLEngine::GLRenderer::Components