#pragma once

#include <Renderer/IRenderableComponent.h>

#include <GLRenderer/Components/TerrainMesh.h>

namespace GLEngine {
namespace GLRenderer {
namespace Components {
class C_TerrainMergedMesh : public Renderer::I_RenderableComponent {
public:
	// @todo fix that, this class is just a hack
	explicit C_TerrainMergedMesh(std::vector<std::shared_ptr<C_TerrainMesh>> meshes);
	~C_TerrainMergedMesh() = default;


	//=======================================================
	virtual void PerformDraw() const override;


	virtual Physics::Primitives::S_AABB GetAABB() const override;

protected:
	std::vector<std::shared_ptr<C_TerrainMesh>> m_Meshes;
};
}}}