#pragma once

#include <Renderer/IRenderCommand.h>

#include <memory>

namespace GLEngine {
namespace GLRenderer {

namespace Mesh {
class C_StaticMeshResource;
}

namespace Commands {
namespace HACK {
// todo: break this hacky solution into commands
class C_DrawStaticMesh : public Renderer::I_RenderCommand {
public:
	C_DrawStaticMesh(std::shared_ptr<Mesh::C_StaticMeshResource> mesh);

	virtual void Commit() override;
	virtual E_Type GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override;

private:
	std::shared_ptr<Mesh::C_StaticMeshResource> m_Mesh;
};
}}}}