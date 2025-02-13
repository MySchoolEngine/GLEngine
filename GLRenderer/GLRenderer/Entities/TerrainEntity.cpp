#include <GLRendererStdafx.h>

#include <GLRenderer/Components/TerrainMergedMesh.h>
#include <GLRenderer/Entities/TerrainEntity.h>

#include <Core/EventSystem/Event.h>

#include <Utils/Range.h>

#include <imgui.h>

namespace GLEngine::GLRenderer {

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
	m_InputCoords = {0, 0};

	int i = 0;
	for (auto& layer : m_Settings.m_Layers)
	{
		layer.m_Name.UpdateText(i);
		++i;
	}

	m_Settings.m_Layers[0].m_TerrainColor.SetValue(glm::vec3(73.f / 255.f, 111.f / 255.f, 134.f / 255.f));
	m_Settings.m_Layers[1].m_TerrainColor.SetValue(glm::vec3(241.f / 255.f, 219.f / 255.f, 101.f / 255.f)); // yellowish color - sand

	m_Settings.m_Layers[0].m_Weight = 0.727f;
	m_Settings.m_Layers[1].m_Weight = 0.043f;
	m_Settings.m_Layers[2].m_Weight = 0.727f;
}

//=================================================================================
C_TerrainEntity::~C_TerrainEntity() = default;

//=================================================================================
Entity::I_Entity::T_ComponentRange C_TerrainEntity::GetComponents(Entity::E_ComponentType type) const
{
	auto iterLow = m_Components->equal_range(type);
	return {Utils::MapValueIterator(std::move(iterLow.first)), Utils::MapValueIterator(std::move(iterLow.second))};
}

//=================================================================================
void C_TerrainEntity::OnEvent(Core::I_Event& event)
{
	Controls = !Controls;
}

//=================================================================================
void C_TerrainEntity::Update()
{
	WholeTerrain([](T_TerrainPtr patch) { patch->UpdateStats(); });

	if (m_SimulationRunning)
	{
		WholeTerrain([&](T_TerrainPtr terrain) { terrain->Simulate(); });
		m_CurrentIteration++;
		if (m_CurrentIteration >= m_Iterations)
		{
			m_SimulationRunning = false;
			CORE_LOG(E_Level::Info, E_Context::Render, "Generation took: {}s and {} have been dropped", m_timer.getElapsedTimeFromLastQueryMilliseconds() / 1000.0,
					 m_Iterations * m_Settings.m_Drops);
		}
	}

	DrawControls();
}

//=================================================================================
void C_TerrainEntity::AddPatch(T_TerrainPtr patch)
{
	patch->SetSettings(&m_Settings);
	m_Patches.emplace(Entity::E_ComponentType::Graphical, patch);
	AddComponent(patch);
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

	if (DebugDrawDroplets)
	{
		WholeTerrain([&](T_TerrainPtr terrain) { terrain->DebugDraw(); });
	}

	if (Controls)
	{
		::ImGui::Begin("Terrain controls", &Controls);
		m_Settings.PerlinNoise.Draw();
		Visualise.Draw();
		DebugDrawDroplets.Draw();

		m_Settings.m_SqPerLine.Draw();
		m_Settings.m_Freq.Draw();
		m_Settings.m_PatchSize.Draw();
		if (m_Settings.m_PatchSize.Changed())
		{
			WholeTerrain([&](T_TerrainPtr terrain) {
				// hacky way how to update model matrix
				terrain->SetCoord(terrain->GetCoord());
			});
		}
		::ImGui::Separator();

		for (auto& layer : m_Settings.m_Layers)
		{
			const auto	Name = layer.m_Name.GetCurrentText();
			std::string ID("##");
			ID.append(Name);
			::ImGui::PushID(ID.data());
			layer.m_Name.Draw();
			layer.m_TerrainColor.Draw();
			layer.m_Weight.Draw();
			::ImGui::PopID();
		}

		::ImGui::Separator();
		if (!m_SimulationRunning)
		{
			m_Settings.m_Gravitation.Draw();
			m_Settings.m_Evaporation.Draw();
			m_Settings.m_InitWater.Draw();
			m_Settings.m_StartingSpeed.Draw();
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
				WholeTerrain([&](T_TerrainPtr terrain) { terrain->Simulate(); });
			}
			::ImGui::SameLine();
			if (::ImGui::Button("Regenerate"))
			{
				WholeTerrain([&](T_TerrainPtr terrain) { terrain->GenerateTerrain(); });
			}

			m_Iterations.Draw();
			m_Settings.m_Drops.Draw();
		}
		else
		{
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
		if (::ImGui::Button("Create"))
		{
			AddPatch(m_InputCoords);
		}

		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Select tiles");
		ImGui::BeginChild("Scrolling");

		WholeTerrain([&](T_TerrainPtr patch) {
			bool			  selected = false;
			auto			  coord	   = patch->GetCoord();
			std::stringstream ss;
			ss << "[" << coord.x << ":" << coord.y << "]";
			::ImGui::Selectable(ss.str().c_str(), &selected);
			if (selected)
			{
				Core::C_UserEvent event("selected");
				patch->OnEvent(event);
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
	// auto range = GetComponents(Entity::E_ComponentType::Graphical);
	std::for_each(Utils::MapValueIterator(m_Patches.begin()), Utils::MapValueIterator(m_Patches.end()), lambda);
}

//=================================================================================
glm::vec3 C_TerrainEntity::GetPosition() const
{
	return glm::vec3(0.f);
}

//=================================================================================
const glm::mat4& C_TerrainEntity::GetModelMatrix() const
{
	const static glm::mat4 identity(1.f);
	return identity;
}

} // namespace GLEngine::GLRenderer