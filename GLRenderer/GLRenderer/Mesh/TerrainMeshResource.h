#pragma once

#include <Renderer/IResource.h>

#include <GLRenderer/Buffers/GLBuffer.h>

namespace GLEngine {
namespace GLRenderer {
namespace Mesh {
class C_TerrainMeshResource : public Renderer::I_Resource {
public:
	C_TerrainMeshResource();
	~C_TerrainMeshResource() = default;

	//=================================================================================
	virtual std::unique_ptr<Renderer::I_RawGPUData> ExtractData() const override;
	virtual void Invalidate() override;
	virtual bool IsValid() override;

	GLuint GetVAO() const;
	std::size_t GetNumTriangles() const;
	std::size_t GetWidth() const;
	std::size_t GetHeight() const;
protected:
	using T_VBO = Buffers::C_GLBuffer<GL_ARRAY_BUFFER>;
	using T_EBO = Buffers::C_GLBuffer<GL_ELEMENT_ARRAY_BUFFER>;

	GLuint m_VAO;
	std::size_t m_triangles;
	std::array<std::shared_ptr<Buffers::I_GLBufferBase>, 4> m_VBOs;
};


}}}