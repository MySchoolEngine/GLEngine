#pragma once

#include <Renderer/IRenderableComponent.h>

#include <GLRenderer/Mesh/TerrainMeshResource.h>
#include <GLRenderer/Textures/Texture.h>

namespace GLEngine {
namespace GLRenderer {
namespace Components {

class C_TerrainMesh : public Renderer::I_RenderableComponent {
public:
	C_TerrainMesh();
	virtual void PerformDraw() const override;
	void SetCoord(glm::ivec2 coord);

	const Textures::C_Texture& GetTexture() const { return m_Noise; }
	Textures::C_Texture& GetTexture() { return m_Noise; }
	void IncreaseFreq() { m_Frequency++; }
	void DecreaseFreq() { m_Frequency--; }
	void IncreaseSQ() { m_SqPerLine++; }
	void DecreaseSQ() { m_SqPerLine--; }
	void UsePerlinNoise(bool val) { m_UsePerlin = val; }
	bool UsingPerlinNoise() const { return m_UsePerlin; }


	//=================================================================================
	virtual void OnEvent(Core::I_Event& event) override;

protected:
	std::shared_ptr<Mesh::C_TerrainMeshResource>				m_Terrain;
	int						m_Frequency;
	int						m_SqPerLine;
	glm::ivec2				m_Coord;
	Textures::C_Texture		m_Noise;
	bool					m_HasTexture : 1;
	bool					m_UsePerlin  : 1;
};

}}}