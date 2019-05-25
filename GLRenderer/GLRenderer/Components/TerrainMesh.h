#pragma once

#include <Renderer/IRenderableComponent.h>

#include <GLRenderer/Buffers/UBO/TerrainStats.h>
#include <GLRenderer/Mesh/TerrainMeshResource.h>
#include <GLRenderer/GUI/CheckBoxValue.h>
#include <GLRenderer/Textures/Texture.h>

namespace GLEngine {
namespace Physics {
namespace Primitives {
struct S_AABB;
}
}

namespace GLRenderer {
namespace Buffers {
namespace UBO {
class C_RainDataBuffer;
}
}

namespace Components {

class C_TerrainMesh : public Renderer::I_RenderableComponent {
public:
	C_TerrainMesh();
	virtual void PerformDraw() const override;
	void SetCoord(glm::ivec2 coord);
	glm::ivec2 GetCoord() const { return m_Coord; }

	const Textures::C_Texture& GetTexture() const { return m_Noise; }
	Textures::C_Texture& GetTexture() { return m_Noise; }
	void SetSqPerLine(int count) { m_SqPerLine = count; }
	void SetFrequncy(int freq);
	void SetNumDrops(int num) { m_NumDrops = num; }
	void UsePerlinNoise(bool val);
	bool UsingPerlinNoise() const { return m_UsePerlin; }

	void UpdateStats();

	void GenerateTerrain();
	void CalculateStats() const;
	void Simulate();
	void DebugDraw();


	//=================================================================================
	virtual void OnEvent(Core::I_Event& event) override;

protected:
	std::shared_ptr<Mesh::C_TerrainMeshResource>		m_Terrain;
	int								m_Frequency;
	int								m_SqPerLine;
	glm::ivec2						m_Coord;
	Textures::C_Texture				m_Noise;
	Physics::Primitives::S_AABB		m_AABB;
	Buffers::UBO::C_TerrainStats	m_Stats;
	std::shared_ptr<Buffers::UBO::C_RainDataBuffer>		m_RainData;
	int								m_NumDrops;
	bool							m_UsePerlin			: 1;
	bool							m_QueuedUpdate		: 1;
	bool							m_QueueSimulation	: 1;
	bool							m_Selected; // cannot be bitfield as I use reference for this boolean
	GUI::C_CheckBoxValue			m_HasTexture;

	glm::mat4 GetModelMatrix() const;

	glm::mat4 m_ModelMatrix;
};

}}}