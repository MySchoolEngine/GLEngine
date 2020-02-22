#pragma once

#include <Renderer/IResource.h>

#include <GLRenderer/VAO/VAO.h>

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

	void BindVAO() const;
	void UnbindVAO() const;
	std::size_t GetNumTriangles() const;
	std::size_t GetWidth() const;
	std::size_t GetHeight() const;
protected:
	std::size_t m_triangles;
	VAO::C_GLVAO<0> m_GLVAO;
};


}}}