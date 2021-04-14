#pragma once

#include <GLRenderer/VAO/VAO.h>

#include <Renderer/IResource.h>

namespace GLEngine::Renderer::MeshData {
struct Mesh;
}

namespace GLEngine::GLRenderer::Mesh {
class C_StaticMeshResource : public Renderer::I_Resource {
public:
	C_StaticMeshResource(const Renderer::MeshData::Mesh& mesh);
	~C_StaticMeshResource() = default;

	//=================================================================================
	virtual std::unique_ptr<Renderer::I_RawGPUData> ExtractData() const override;
	virtual void									Invalidate() override;
	virtual bool									IsValid() override;

	void		BindVAO() const;
	std::size_t GetNumTriangles() const;

protected:
	VAO::C_GLVAO<5> m_VAO;
	std::size_t		m_triangles;
	std::string		m_Name;
};
} // namespace GLEngine::GLRenderer::Mesh
