#pragma once

#include <GLRenderer/VAO/VAO.h>

namespace GLEngine::GLRenderer::Mesh {
class C_TerrainMeshResource {
public:
	C_TerrainMeshResource();
	~C_TerrainMeshResource() = default;

	void		BindVAO() const;
	void		UnbindVAO() const;
	std::size_t GetNumTriangles() const;
	std::size_t GetWidth() const;
	std::size_t GetHeight() const;

protected:
	VAO::C_GLVAO<0> m_GLVAO;
};


} // namespace GLEngine::GLRenderer::Mesh