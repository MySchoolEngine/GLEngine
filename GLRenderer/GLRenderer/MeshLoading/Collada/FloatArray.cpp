#include <GLRendererStdafx.h>

#include <GLRenderer/MeshLoading/Collada/FloatArray.h>

#include <pugixml.hpp>

namespace GLEngine::GLRenderer::Mesh {

//=================================================================================
S_FloatArray::S_FloatArray(const pugi::xml_node& floatArray)
	: m_Position(0)
{
	const std::string_view floats = floatArray.child_value();
	m_FloatsStream << floats;
	m_NumFloats = floatArray.attribute("count").as_int();
}

//=================================================================================
bool S_FloatArray::EndOfArray() const
{
	return m_Position >= m_NumFloats;
}

}