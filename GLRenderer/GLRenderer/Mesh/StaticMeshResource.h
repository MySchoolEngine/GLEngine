#pragma once

#include <GLRenderer/VAO/VAO.h>

namespace GLEngine::Renderer::MeshData {
struct Mesh;
}

namespace GLEngine::GLRenderer::Mesh {
class C_StaticMeshResource {
public:
	C_StaticMeshResource(const Renderer::MeshData::Mesh& mesh);
	~C_StaticMeshResource() = default;

	void		BindVAO() const;
	std::size_t GetNumTriangles() const;

protected:
	VAO::C_GLVAO<5> m_VAO;
	std::size_t		m_triangles;
	std::string		m_Name;
};
} // namespace GLEngine::GLRenderer::Mesh
