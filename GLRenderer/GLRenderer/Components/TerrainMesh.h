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
	const Textures::C_Texture& GetTexture() const { return m_Noise; }
	Textures::C_Texture& GetTexture() { return m_Noise; }
	void IncreaseFreq() { m_Frequency++; }
	void DecreaseFreq() { m_Frequency--; }
protected:
	std::shared_ptr<Mesh::C_TerrainMeshResource>				m_Terrain;
	int						m_Frequency;
	Textures::C_Texture		m_Noise;
};

}}}