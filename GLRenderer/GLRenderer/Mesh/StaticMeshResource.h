#pragma once

#include <Renderer/IResource.h>

#include <GLRenderer/VAO/VAO.h>

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

	void BindVAO() const;
	std::size_t GetNumTriangles() const;
protected:
	VAO::C_GLVAO<3> m_VAO;
	std::size_t m_triangles;
};


}}}