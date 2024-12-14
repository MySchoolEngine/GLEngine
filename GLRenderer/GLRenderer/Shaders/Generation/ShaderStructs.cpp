#include <GLRendererStdafx.h>

#include <GLRenderer/Shaders/Generation/ShaderStructs.h>

namespace GLEngine::GLRenderer::Shaders {

//=================================================================================
// C_MemberDescriptor
//=================================================================================
std::string S_MemberDescriptor::Generate() const
{
	return std::string(m_Type) + '\t' + std::string(m_Name) + ";";
}

//=================================================================================
// C_StructDescriptor
//=================================================================================
C_StructDescriptor::C_StructDescriptor(const std::string_view name)
	: m_Name(name)
{
}

//=================================================================================
std::string C_StructDescriptor::Generate() const
{
	std::string ret;
	ret += "struct " + std::string(m_Name) + " {\n";
	for (const auto& it : m_Members)
	{
		ret += '\t' + it.Generate() + '\n';
	}
	ret += "}";
	return ret;
}

//=================================================================================
void C_StructDescriptor::Push(S_MemberDescriptor description)
{
	m_Members.emplace_back(description);
}

} // namespace GLEngine::GLRenderer::Shaders