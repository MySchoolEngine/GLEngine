#pragma once

#include <Entity/IEntity.h>

#include <GLRenderer/GUI/CheckBoxValue.h>

namespace GLEngine {
namespace GLRenderer {

namespace Components {
class C_TerrainMesh;
}

class C_TerrainEntity : public Entity::I_Entity {
public:
	C_TerrainEntity();
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
		int m_SqPerLine			= 32;
		int m_Freq				= 5;
		int m_Drops				= 100;
		int m_NumSteps			= 90;
		GUI::C_CheckBoxValue PerlinNoise = true;
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
	int  m_Iterations;
	int  m_CurrentIteration;

	S_TerrainSettings m_Settings;
	bool Controls;
	GUI::C_CheckBoxValue Visualise;
	GUI::C_CheckBoxValue DebugDrawDroplets;
};

}
}