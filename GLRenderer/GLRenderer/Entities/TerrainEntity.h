#pragma once

#include <Entity/IEntity.h>

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
protected:
	EntityID					m_ID;
	std::string					m_Name;
	std::vector<T_TerrainPtr>	m_Patches;

	int m_SqPerLine;
	int m_Freq;
	int m_Drops;

	bool Controls;
	bool Visualise;
	bool PerlinNoise;
	bool DebugDrawDroplets;
};

}
}