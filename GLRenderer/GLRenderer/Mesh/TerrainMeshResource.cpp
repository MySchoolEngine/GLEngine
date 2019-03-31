#include <GLRendererStdafx.h>

#include <GLRenderer/Mesh/TerrainMeshResource.h>


namespace GLEngine {
namespace GLRenderer {
namespace Mesh {

//=================================================================================
C_TerrainMeshResource::C_TerrainMeshResource()
{
	glGenVertexArrays(1, &m_VAO);

	m_VBOs[0] = std::make_shared<T_VBO>();
	m_VBOs[1] = std::make_shared<T_VBO>();
	m_VBOs[2] = std::make_shared<T_VBO>();
	m_VBOs[3] = std::make_shared<T_EBO>();

	glBindVertexArray(m_VAO);

	// bind VBO in order to use
	m_VBOs[0]->bind();

	static_assert(sizeof(glm::vec3) == sizeof(GLfloat) * 3, "Platform doesn't support this directly.");
	std::vector<float> vertices;
	vertices.push_back(0);
	vertices.push_back(0);
	vertices.push_back(0);
	vertices.push_back(0);
	vertices.push_back(0);
	vertices.push_back(0);
	vertices.push_back(0);
	vertices.push_back(0);
	vertices.push_back(0);
	std::vector<glm::vec3> normals;
	normals.emplace_back(0, 1, 0);
	std::vector<glm::vec2> texcoords;

	texcoords.emplace_back(0, 1);
	texcoords.emplace_back(0, 0);
	texcoords.emplace_back(1, 1);
	texcoords.emplace_back(0, 0);
	texcoords.emplace_back(1, 0);
	texcoords.emplace_back(1, 1);

	std::vector<int> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(0xFFFF);
	indices.push_back(1);
	indices.push_back(6);
	indices.push_back(3);
	indices.push_back(7);
	indices.push_back(5);
	indices.push_back(8);
	indices.push_back(0xFFFF);

	// upload data to VBO
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(decltype(vertices)::value_type), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, nullptr);


	m_VBOs[1]->bind();
	// upload data to VBO
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(decltype(normals)::value_type), normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	m_VBOs[2]->bind();
	// upload data to VBO
	glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(decltype(texcoords)::value_type), texcoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	m_VBOs[3]->bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(decltype(indices)::value_type), indices.data(), GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	m_VBOs[2]->unbind();
	glBindVertexArray(0);
}

//=================================================================================
std::unique_ptr<GLEngine::Renderer::I_RawGPUData> C_TerrainMeshResource::ExtractData() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_TerrainMeshResource::Invalidate()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
bool C_TerrainMeshResource::IsValid()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
GLuint C_TerrainMeshResource::GetVAO() const
{
	return m_VAO;
}

//=================================================================================
std::size_t C_TerrainMeshResource::GetNumTriangles() const
{
	return 14;
}

//=================================================================================
std::size_t C_TerrainMeshResource::GetWidth() const
{
	return 2;
}

//=================================================================================
std::size_t C_TerrainMeshResource::GetHeight() const
{
	return 2;
}

}}}