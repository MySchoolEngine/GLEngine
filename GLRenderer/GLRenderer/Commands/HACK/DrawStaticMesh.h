#pragma once

#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer {

namespace Mesh {
class C_StaticMeshResource;
}

namespace Commands::HACK {
// todo: break this hacky solution into commands
class C_DrawStaticMesh : public Renderer::I_RenderCommand {
public:
	explicit C_DrawStaticMesh(std::shared_ptr<Mesh::C_StaticMeshResource> mesh);

	virtual void								  Commit() override;
	virtual E_Type								  GetType() const override;
	[[nodiscard]] virtual std::string			  GetDescriptor() const override;

private:
	std::shared_ptr<Mesh::C_StaticMeshResource> m_Mesh;
};
} // namespace Commands::HACK
} // namespace GLEngine::GLRenderer