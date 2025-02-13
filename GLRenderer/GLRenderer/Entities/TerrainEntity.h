#pragma once

#include <GUI/Input/CheckBoxValue.h>
#include <GUI/Input/Color.h>
#include <GUI/Input/Slider.h>
#include <GUI/Text.h>

#include <Entity/IEntity.h>

#include <Utils/HighResolutionTimer.h>

namespace GLEngine::GLRenderer {

namespace Components {
class C_TerrainMesh;
}

class C_TerrainEntity : public Entity::I_Entity {
public:
	C_TerrainEntity(const std::string& name = "Terrain");
	~C_TerrainEntity() override;

	using T_TerrainPtr = std::shared_ptr<Components::C_TerrainMesh>;

	//=================================================================================
	[[nodiscard]] Entity::I_Entity::T_ComponentRange GetComponents(Entity::E_ComponentType type) const override;
	void											 OnEvent(Core::I_Event& event) override;

	void Update() override;

	void AddPatch(T_TerrainPtr);
	void AddPatch(glm::ivec2 coord);

	void DrawControls();

	void WholeTerrain(std::function<void(T_TerrainPtr)> lambda);

	struct S_LayerSettings {
		GUI::C_FormatedText			m_Name{"Layer {}"};
		GUI::Input::C_ColorRBG		m_TerrainColor{"Layer color", glm::vec3(0.254f, 0.6470f, 0.1921f)};
		GUI::Input::C_Slider<float> m_Weight{.5f, 0.f, 1.f, "Weight"};
	};

	struct S_TerrainSettings {
		GUI::Input::C_Slider<int>	m_SqPerLine{255, 4, 1024, "Vertices"};
		GUI::Input::C_Slider<int>	m_Freq{24, 4, 40, "Noise frequency"};
		GUI::Input::C_Slider<int>	m_Drops{100, 1, 100, "# Drops"};
		GUI::Input::C_Slider<int>	m_NumSteps{90, 30, 150, "Max lifetime"};
		GUI::Input::C_Slider<int>	m_PatchSize{40, 4, 60, "Patch size"};
		GUI::Input::C_CheckBoxValue PerlinNoise{true, "Use Perlin noise"};
		S_LayerSettings				m_Layers[3];
		GUI::Input::C_Slider<float> m_Evaporation{0.02f, 0.f, 1.f, "Evaporation"};
		GUI::Input::C_Slider<float> m_Gravitation{4.0f, 0.f, 15.f, "Gravitation"};
		GUI::Input::C_Slider<float> m_InitWater{1.f, 0.1f, 5.0f, "InitWater"};
		GUI::Input::C_Slider<float> m_StartingSpeed{2.0f, 0.0f, 10.0f, "InitSpeed"};
		float						m_Inertia = 0.65f;
	};

	[[nodiscard]] glm::vec3 GetPosition() const override;


	//=================================================================================
	const glm::mat4& GetModelMatrix() const override;

protected:
	std::multimap<Entity::E_ComponentType, T_TerrainPtr> m_Patches;

	glm::ivec2				  m_InputCoords;
	bool					  m_SimulationRunning;
	GUI::Input::C_Slider<int> m_Iterations{1000, 1, 10000, "Iterations"};
	int						  m_CurrentIteration;

	S_TerrainSettings			m_Settings;
	bool						Controls;
	GUI::Input::C_CheckBoxValue Visualise;
	GUI::Input::C_CheckBoxValue DebugDrawDroplets;
	Utils::HighResolutionTimer	m_timer;
};

} // namespace GLEngine::GLRenderer