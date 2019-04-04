#include <GLRendererStdafx.h>

#include <GLRenderer/Mesh/TerrainMeshResource.h>


namespace GLEngine {
namespace GLRenderer {
namespace Mesh {

//=================================================================================
C_TerrainMeshResource::C_TerrainMeshResource()
{
	m_GLVAO.bind();

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

	//indices.push_back(0);
	//indices.push_back(1);
	//indices.push_back(3);
	//indices.push_back(2);


	m_GLVAO.SetBuffer<0, GL_ARRAY_BUFFER>(vertices);
	m_GLVAO.SetBuffer<1, GL_ARRAY_BUFFER>(normals);
	m_GLVAO.SetBuffer<2, GL_ARRAY_BUFFER>(texcoords);
	m_GLVAO.SetBuffer<3, GL_ELEMENT_ARRAY_BUFFER>(indices);

	m_GLVAO.BindBuffer<3>();

	m_GLVAO.EnableArray<0>();
	m_GLVAO.EnableArray<1>();
	m_GLVAO.EnableArray<2>();

	m_GLVAO.unbind();
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
void C_TerrainMeshResource::BindVAO() const
{
	m_GLVAO.bind();
}

//=================================================================================
void C_TerrainMeshResource::UnbindVAO() const
{
	m_GLVAO.unbind();
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