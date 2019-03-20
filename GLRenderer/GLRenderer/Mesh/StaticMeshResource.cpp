#include <GLRendererStdafx.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>

#include <GLRenderer/MeshLoading/Scene.h>

namespace GLEngine {
namespace GLRenderer {
namespace Mesh {
//=================================================================================
C_StaticMeshResource::C_StaticMeshResource(const Mesh & mesh)
{
	glGenVertexArrays(1, &m_VAO);

	m_VBOs[0] = std::make_shared<T_VBO>();
	m_VBOs[1] = std::make_shared<T_VBO>();
	m_VBOs[2] = std::make_shared<T_VBO>();


	m_triangles = static_cast<GLuint>(mesh.vertices.size());

	glBindVertexArray(m_VAO);

	// bind VBO in order to use
	m_VBOs[0]->bind();

	static_assert(sizeof(glm::vec3) == sizeof(GLfloat) * 3, "Platform doesn't support this directly.");

	// upload data to VBO
	glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec4), mesh.vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);


	m_VBOs[1]->bind();
	// upload data to VBO
	glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(glm::vec3), mesh.normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	m_VBOs[2]->bind();
	// upload data to VBO
	glBufferData(GL_ARRAY_BUFFER, mesh.texcoords.size() * sizeof(glm::vec2), mesh.texcoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	m_VBOs[2]->unbind();
	glBindVertexArray(0);
}

//=================================================================================
std::unique_ptr<GLEngine::Renderer::I_RawGPUData> C_StaticMeshResource::ExtractData() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_StaticMeshResource::Invalidate()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
bool C_StaticMeshResource::IsValid()
{
	throw std::logic_error("The method or operation is not implemented.");
}

}
}
}

