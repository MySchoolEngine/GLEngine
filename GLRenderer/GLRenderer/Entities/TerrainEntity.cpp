#include <GLRendererStdafx.h>

#include <GLRenderer/Entities/TerrainEntity.h>

#include <GLRenderer/Components/TerrainMergedMesh.h>

#include <imgui.h>

namespace GLEngine {
namespace GLRenderer {

//=================================================================================
C_TerrainEntity::C_TerrainEntity()
	: m_ID(NextGUID())
	, Controls(false)
	, Visualise(false)
	, DebugDrawDroplets(false)
{
	Visualise.SetName("Visualization");
	m_Settings.PerlinNoise.SetName("Use Perlin noise");
	DebugDrawDroplets.SetName("Debug draw");
	m_inputCoords[0] = 0;
	m_inputCoords[1] = 0;
}

//=================================================================================
GLEngine::Entity::I_Entity::EntityID C_TerrainEntity::GetID() const
{
	return m_ID;
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
std::string C_TerrainEntity::GetName() const
{
	return "Terrain";
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
}

//=================================================================================
void C_TerrainEntity::AddPatch(T_TerrainPtr patch)
{
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
	if (Controls) {
		::ImGui::Begin("Terrain controls", &Controls, ImGuiWindowFlags_MenuBar);
			if (::ImGui::BeginMenuBar())
			{
				if (::ImGui::BeginMenu("File"))
				{
					if (::ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
					if (::ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
					::ImGui::EndMenu();
				}
				::ImGui::EndMenuBar();
			}
			if (::ImGui::Button("Rain"))
			{
				WholeTerrain([&](T_TerrainPtr terrain) {
					terrain->Simulate();
				});
			}
			::ImGui::SliderInt("Drops", &(m_Settings.m_Drops), 1, 100);
			if (::ImGui::Button("Regenerate"))
			{
				WholeTerrain([&](T_TerrainPtr terrain) {
					terrain->GenerateTerrain();
				});
			}
			m_Settings.PerlinNoise.Draw();
			Visualise.Draw();
			DebugDrawDroplets.Draw();

			if (DebugDrawDroplets) {
				WholeTerrain([&](T_TerrainPtr terrain) {
					terrain->DebugDraw();
				});
			}

			::ImGui::SliderInt("Verticies", &(m_Settings.m_SqPerLine), 4, 64);
			::ImGui::SliderInt("Noise frequency", &(m_Settings.m_Freq), 4, 40);
			::ImGui::Separator();
			::ImGui::SliderFloat("Gravitation", &(m_Settings.m_Gravitation), 0.0, 15.0);
			::ImGui::SliderFloat("Evaporation", &(m_Settings.m_Evaporation), 0.0, 1.0);
			::ImGui::SliderFloat("InitWater", &(m_Settings.m_InitWater), 0.1, 5.0);
			::ImGui::SliderFloat("InitSpeed", &(m_Settings.m_StartingSpeed), 0.0, 10.0);

			::ImGui::Separator();

			int* arr[] = { &m_inputCoords[0],&m_inputCoords[1] };
			::ImGui::SliderInt("X", &(m_inputCoords[0]), -10, 10);
			::ImGui::SliderInt("Y", &(m_inputCoords[1]), -10, 10);
			if (::ImGui::Button("Create")) {
				AddPatch({ m_inputCoords[0],m_inputCoords[1] });
			}

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
		::ImGui::End();
	}


	if (Visualise) {
		::ImGui::Begin("Terrain visualizations", &Visualise);
			WholeTerrain([](T_TerrainPtr terrain) {
				::ImGui::Image((void*)terrain->GetTexture().GetTexture(),
				{
					256,
					256
				},
				{ 0,1 }, { 1,0 });
			});
		::ImGui::End();
	}
}


//=================================================================================
void C_TerrainEntity::WholeTerrain(std::function<void(T_TerrainPtr)> lambda)
{
	std::for_each(m_Patches.begin(), m_Patches.end(), lambda);
}

}}