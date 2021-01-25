#pragma once

#include <GLRenderer/Components/TerrainMesh.h>

#include <Renderer/IRenderableComponent.h>

namespace GLEngine { namespace GLRenderer { namespace Components {
class C_TerrainMergedMesh : public Renderer::I_RenderableComponent {
public:
	// @todo fix that, this class is just a hack
	explicit C_TerrainMergedMesh(std::vector<std::shared_ptr<C_TerrainMesh>> meshes);
	~C_TerrainMergedMesh() = default;


	//=======================================================
	virtual void						PerformDraw() const override;
	virtual Physics::Primitives::S_AABB GetAABB() const override;

	virtual std::string_view GetDebugComponentName() const override;
	virtual bool			 HasDebugDrawGUI() const override;

protected:
	std::vector<std::shared_ptr<C_TerrainMesh>>	 m_Meshes;
	static constexpr Physics::Primitives::S_AABB m_AABB = Physics::Primitives::S_AABB();
};
}}} // namespace GLEngine::GLRenderer::Components