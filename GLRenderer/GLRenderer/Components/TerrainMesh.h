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
protected:
	std::shared_ptr<Mesh::C_TerrainMeshResource>				m_Terrain;
	int						m_Frequency;
	int						m_SqPerLine;
	glm::ivec2				m_Coord;
	Textures::C_Texture		m_Noise;
};

}}}