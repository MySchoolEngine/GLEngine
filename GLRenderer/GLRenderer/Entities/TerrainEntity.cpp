#include <GLRendererStdafx.h>

#include <GLRenderer/Entities/TerrainEntity.h>

#include <GLRenderer/Components/TerrainMergedMesh.h>

#include <imgui.h>

namespace GLEngine {
namespace GLRenderer {

//=================================================================================
C_TerrainEntity::C_TerrainEntity(const std::string& name)
	: I_Entity(name)
	, m_SimulationRunning(false)
	, m_CurrentIteration(0)
	, Controls(false)
	, Visualise(false)
	, DebugDrawDroplets(false)
{
	Visualise.SetName("Visualization");
	DebugDrawDroplets.SetName("Debug draw");
	m_InputCoords = { 0,0 };
}

//=================================================================================
GLEngine::T_ComponentPtr C_TerrainEntity::GetComponent(Entity::E_ComponentType type) const
{
	if (type == Entity::E_ComponentType::Graphical) {
		// should be frustum culled
		auto merged = std::make_shared<Components::C_TerrainMergedMesh>(m_Patches);
		return merged;
	}

	return nullptr;
}

//=================================================================================
void C_TerrainEntity::OnEvent(Core::I_Event& event)
{
	Controls = !Controls;
}

//=================================================================================
void C_TerrainEntity::Update()
{
	WholeTerrain([](T_TerrainPtr patch) {
		patch->UpdateStats();
	});

	if (m_SimulationRunning) {
		WholeTerrain([&](T_TerrainPtr terrain) {
			terrain->Simulate();
		});
		m_CurrentIteration++;
		if (m_CurrentIteration >= m_Iterations) {
			m_SimulationRunning = false;
			CORE_LOG(E_Level::Info, E_Context::Render, "Generation took: {}s and {} have been dropped", m_timer.getElapsedTimeFromLastQueryMilliseconds() / 1000.0, m_Iterations * m_Settings.m_Drops);
		}
	}

	DrawControls();
}

//=================================================================================
void C_TerrainEntity::AddPatch(T_TerrainPtr patch)
{
	patch->SetSettings(&m_Settings);
	m_Patches.push_back(patch);
}

//=================================================================================
void C_TerrainEntity::AddPatch(glm::ivec2 coord)
{
	auto comp = std::make_shared<Components::C_TerrainMesh>(&m_Settings);
	comp->SetCoord(coord);
	AddPatch(comp);
}

//=================================================================================
void C_TerrainEntity::DrawControls()
{

	if (DebugDrawDroplets) {
		WholeTerrain([&](T_TerrainPtr terrain) {
			terrain->DebugDraw();
			});
	}

	if (Controls) {
		::ImGui::Begin("Terrain controls", &Controls);
			m_Settings.PerlinNoise.Draw();
			Visualise.Draw();
			DebugDrawDroplets.Draw();

			m_Settings.m_SqPerLine.Draw();
			m_Settings.m_Freq.Draw();
			m_Settings.m_PatchSize.Draw();
			if (m_Settings.m_PatchSize.Changed()) {
				WholeTerrain([&](T_TerrainPtr terrain) {
					// hacky way how to update model matrix
					terrain->SetCoord(terrain->GetCoord());
				});
			}
			::ImGui::Separator();
			::ImGui::Text("Colors");
			m_Settings.m_TerrainColor.Draw();

			::ImGui::Separator();
			if(!m_SimulationRunning)
			{
				::ImGui::SliderFloat("Gravitation", &(m_Settings.m_Gravitation), 0.0f, 15.0f);
				::ImGui::SliderFloat("Evaporation", &(m_Settings.m_Evaporation), 0.0f, 1.0f);
				::ImGui::SliderFloat("InitWater", &(m_Settings.m_InitWater), 0.1f, 5.0f);
				::ImGui::SliderFloat("InitSpeed", &(m_Settings.m_StartingSpeed), 0.0f, 10.0f);
				m_Settings.m_NumSteps.Draw();

				if (::ImGui::Button("Simulate"))
				{
					m_SimulationRunning = true;
					m_timer.reset();
					m_CurrentIteration = 0;
				}
				::ImGui::SameLine();
				if (::ImGui::Button("Rain"))
				{
					WholeTerrain([&](T_TerrainPtr terrain) {
						terrain->Simulate();
					});
				}
				::ImGui::SameLine();
				if (::ImGui::Button("Regenerate"))
				{
					WholeTerrain([&](T_TerrainPtr terrain) {
						terrain->GenerateTerrain();
					});
				}

				m_Iterations.Draw();
				m_Settings.m_Drops.Draw();
			}
			else {
				::ImGui::TextColored(ImVec4(1, 0, 0, 1), "Simulating");
				::ImGui::ProgressBar(static_cast<float>(m_CurrentIteration) / m_Iterations);
				if (::ImGui::Button("Stop simulation"))
				{
					m_SimulationRunning = false;
				}
			}

			::ImGui::Separator();

			::ImGui::Text("Spawning new tiles");

			::ImGui::SliderInt("X", &(m_InputCoords.x), -10, 10);
			::ImGui::SliderInt("Y", &(m_InputCoords.y), -10, 10);
			if (::ImGui::Button("Create")) {
				AddPatch(m_InputCoords);
			}

			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Select tiles");
			ImGui::BeginChild("Scrolling");

			WholeTerrain([&](T_TerrainPtr patch) {
				bool selected = false;
				auto coord = patch->GetCoord();
				std::stringstream ss;
				ss << "[" << coord.x << ":" << coord.y << "]";
				::ImGui::Selectable(ss.str().c_str(), &selected);
				if (selected) {
					patch->OnEvent(Core::C_UserEvent("selected"));
				}
			});
			ImGui::EndChild();
		::ImGui::End();
	}


	// if (Visualise) {
	// 	::ImGui::Begin("Terrain visualizations", &Visualise);
	// 		WholeTerrain([](T_TerrainPtr terrain) {
	// 			::ImGui::Image((void*)terrain->GetTexture().GetTexture(),
	// 			{
	// 				256,
	// 				256
	// 			},
	// 			{ 0,1 }, { 1,0 });
	// 		});
	// 	::ImGui::End();
	// }
}

//=================================================================================
void C_TerrainEntity::WholeTerrain(std::function<void(T_TerrainPtr)> lambda)
{
	std::for_each(m_Patches.begin(), m_Patches.end(), lambda);
}

//=================================================================================
glm::vec3 C_TerrainEntity::GetPosition() const
{
	return glm::vec3(0.f);
}

}}