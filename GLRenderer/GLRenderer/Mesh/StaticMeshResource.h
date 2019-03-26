#pragma once

#include <Renderer/IResource.h>

#include <GLRenderer/Buffers/GLBuffer.h>

#include <glad/glad.h>

#include <array>

namespace GLEngine {
namespace GLRenderer {
namespace Mesh {
struct Mesh;
class C_StaticMeshResource : public Renderer::I_Resource {
public:
	C_StaticMeshResource(const Mesh & mesh);
	~C_StaticMeshResource() = default;

	//=================================================================================
	virtual std::unique_ptr<Renderer::I_RawGPUData> ExtractData() const override;
	virtual void Invalidate() override;
	virtual bool IsValid() override;

	GLuint GetVAO() const;
protected:
	using T_VBO = Buffers::C_GLBuffer<GL_ARRAY_BUFFER>;

	GLuint m_VAO;
	GLuint m_triangles;
	std::array<std::shared_ptr<T_VBO>, 3> m_VBOs;
};


}}}