#include <GLRendererStdafx.h>

#include <GLRenderer/Shaders/Generation/ShaderStructs.h>

namespace GLEngine::GLRenderer::Shaders {

//=================================================================================
// C_MemberDescriptor
//=================================================================================
S_MemberDescriptor::S_MemberDescriptor(std::string name, std::string type)
	: m_Name(name)
	, m_Type(type)
{

}

//=================================================================================
std::string S_MemberDescriptor::Generate() const
{
	return m_Type + " " + m_Name + ";";
}

//=================================================================================
// C_StructDescriptor
//=================================================================================
C_StructDescriptor::C_StructDescriptor(std::string name)
	: m_Name(name)
{

}

//=================================================================================
std::string C_StructDescriptor::Generate() const
{
	std::string ret;
	ret += "struct " + m_Name + " {\n";
	for (const auto& it : m_Members)
	{
		ret += '\t' + it.Generate() + '\n';
	}
	ret += "}\n";
	return ret;
}

//=================================================================================
void C_StructDescriptor::Push(S_MemberDescriptor description)
{
	m_Members.emplace_back(description);
}

}