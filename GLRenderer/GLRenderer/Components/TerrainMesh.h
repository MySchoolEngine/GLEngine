#pragma once

#include <GLRenderer/Buffers/UBO/TerrainStats.h>
#include <GLRenderer/Entities/TerrainEntity.h>
#include <GLRenderer/Mesh/TerrainMeshResource.h>
#include <GLRenderer/Textures/Texture.h>

#include <Renderer/IRenderableComponent.h>

#include <GUI/Input/CheckBoxValue.h>

namespace GLEngine {
namespace Physics::Primitives {
struct S_AABB;
} // namespace Physics::Primitives

namespace GLRenderer {
namespace Buffers::UBO {
class C_RainDataBuffer;
} // namespace Buffers::UBO

namespace Components {

class C_TerrainMesh : public Renderer::I_RenderableComponent {
public:
	explicit C_TerrainMesh(C_TerrainEntity::S_TerrainSettings* settings);
	explicit C_TerrainMesh(Textures::C_Texture&& texture);

	void					 SetSettings(C_TerrainEntity::S_TerrainSettings* settings);
	virtual void			 PerformDraw() const override;
	void					 SetCoord(glm::ivec2 coord);
	[[nodiscard]] glm::ivec2 GetCoord() const { return m_Coord; }

	[[nodiscard]] const Textures::C_Texture& GetTexture() const { return m_Noise; }
	[[nodiscard]] Textures::C_Texture&		 GetTexture() { return m_Noise; }
	[[nodiscard]] bool						 UsingPerlinNoise() const { return m_Settings->PerlinNoise; }

	void UpdateStats();

	void GenerateTerrain();
	void Simulate();
	void DebugDraw();


	virtual std::string_view GetDebugComponentName() const override;
	virtual bool			 HasDebugDrawGUI() const override;
	//=================================================================================
	virtual void OnEvent(Core::I_Event& event) override;

	//=================================================================================
	[[nodiscard]] virtual Physics::Primitives::S_AABB GetAABB() const override;

protected:
	std::shared_ptr<Mesh::C_TerrainMeshResource>	m_Terrain;
	glm::ivec2										m_Coord;
	mutable Textures::C_Texture						m_Noise;
	Physics::Primitives::S_AABB						m_AABB;
	Buffers::UBO::C_TerrainStats					m_Stats;
	std::shared_ptr<Buffers::UBO::C_RainDataBuffer> m_RainData;
	C_TerrainEntity::S_TerrainSettings*				m_Settings;
	bool											m_QueuedUpdate : 1;
	bool											m_QueueSimulation : 1;
	bool											m_Selected; // cannot be bitfield as I use reference for this boolean
	GUI::Input::C_CheckBoxValue						m_HasTexture;

	glm::mat4 GetModelMatrix() const;

	glm::mat4 m_ModelMatrix;
};

} // namespace Components
} // namespace GLRenderer
} // namespace GLEngine