#pragma once

namespace GLEngine::GLRenderer::Shaders {

//=================================================================================
class S_MemberDescriptor
{
public:
	S_MemberDescriptor(std::string name, std::string type);
	std::string Generate() const;

	std::string m_Name;
	std::string m_Type;
};

//=================================================================================
class C_StructDescriptor
{
public:
	C_StructDescriptor(std::string name);
	std::string Generate() const;
	void Push(S_MemberDescriptor description);

private:
	std::string m_Name;
	std::vector<S_MemberDescriptor> m_Members;
};

//=================================================================================
class I_GLSLStruct
{
public:
	virtual C_StructDescriptor GetDescription() const = 0;
	virtual std::string GetName() const = 0;
};
}