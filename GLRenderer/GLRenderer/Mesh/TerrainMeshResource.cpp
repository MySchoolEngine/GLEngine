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
	std::vector<float> vertices(16);
	for (int i = 0; i < 16; ++i) {
		vertices.push_back(0);
	}

	vertices[3] = 0.5f;
	vertices[5] = 0.5f;
	vertices[10] = 0.5f;
	vertices[12] = 0.5f;

	std::vector<glm::vec3> normals;
	normals.emplace_back(0, 1, 0);
	std::vector<glm::vec2> texcoords;

	texcoords.emplace_back(0, 1);
	texcoords.emplace_back(0, 0);
	texcoords.emplace_back(1, 1);
	texcoords.emplace_back(0, 0);
	texcoords.emplace_back(1, 0);
	texcoords.emplace_back(1, 1);

	auto startOfLine = [=](int i) {
		auto base = static_cast<int>((i / 2)*GetWidth() * 2);

		return base + i % 2;
	};

	std::vector<int> indices;
	for (int i = 0; i < GetHeight() - 1; ++i) {
		auto firstLineStart = startOfLine(i);
		auto secondLineStart = startOfLine(i+1);
		for (int j = 0; j < GetWidth(); ++j) {
			indices.push_back(firstLineStart + j * 2);
			indices.push_back(secondLineStart + j * 2);
		}
		indices.push_back(0xFFFF);
	}

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
	return GetWidth()*2*(GetHeight()-1) + GetHeight() - 1;
}

//=================================================================================
std::size_t C_TerrainMeshResource::GetWidth() const
{
	return 4;
}

//=================================================================================
std::size_t C_TerrainMeshResource::GetHeight() const
{
	return 4;
}

}}}