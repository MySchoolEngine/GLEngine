#pragma once

#include <Entity/IEntity.h>

#include <GLRenderer/GUI/CheckBoxValue.h>
#include <GLRenderer/GUI/Slider.h>

#include <Utils/HighResolutionTimer.h>

namespace GLEngine {
namespace GLRenderer {

namespace Components {
class C_TerrainMesh;
}

class C_TerrainEntity : public Entity::I_Entity {
public:
	C_TerrainEntity(const std::string& name = "Terrain");
	~C_TerrainEntity() = default;

	using T_TerrainPtr = std::shared_ptr<Components::C_TerrainMesh>;

	//=================================================================================
	virtual EntityID GetID() const override;
	virtual T_ComponentPtr GetComponent(Entity::E_ComponentType type) const override;
	virtual std::string GetName() const override;
	virtual void OnEvent(Core::I_Event& event) override;

	void Update();

	void AddPatch(T_TerrainPtr);
	void AddPatch(glm::ivec2 coord);

	void DrawControls();

	void WholeTerrain(std::function<void(T_TerrainPtr)> lambda);

	struct S_TerrainSettings {
		GUI::C_Slider<int> m_SqPerLine	{ 255, 4, 255, "Vertices" };
		GUI::C_Slider<int> m_Freq		{ 24, 4, 40, "Noise frequency" };
		GUI::C_Slider<int> m_Drops		{ 100, 1, 100, "# Drops" };
		GUI::C_Slider<int> m_NumSteps   { 90, 30, 150, "Max lifetime" };
		GUI::C_Slider<int>	m_PatchSize	{ 40, 4, 60, "Patch size"};
		GUI::C_CheckBoxValue PerlinNoise{ true, "Use Perlin noise" };
		float m_Evaporation		= 0.02f;
		float m_Gravitation		= 4.0f;
		float m_InitWater		= 1.0f;
		float m_StartingSpeed	= 2.0f;
		float m_Inertia			= 0.65f;
	};
protected:
	EntityID					m_ID;
	std::string					m_Name;
	std::vector<T_TerrainPtr>	m_Patches;

	int m_inputCoords[2];
	bool m_SimulationRunning;
	GUI::C_Slider<int>  m_Iterations {1000, 1, 1000000, "Iterations"};
	int  m_CurrentIteration;

	S_TerrainSettings m_Settings;
	bool Controls;
	GUI::C_CheckBoxValue Visualise;
	GUI::C_CheckBoxValue DebugDrawDroplets;
	Utils::HighResolutionTimer m_timer;
};

}
}