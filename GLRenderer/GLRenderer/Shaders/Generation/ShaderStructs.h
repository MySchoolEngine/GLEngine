#pragma once

namespace GLEngine::GLRenderer::Shaders {

//=================================================================================
class S_MemberDescriptor
{
public:
	constexpr S_MemberDescriptor(std::string_view name, std::string_view type);
	[[nodiscard]] std::string Generate() const;

	std::string_view	m_Name;
	std::string_view	m_Type;
};

//=================================================================================
class C_StructDescriptor
{
public:
	C_StructDescriptor(std::string_view name);
	[[nodiscard]] std::string Generate() const;

	template<class T, class MemType>
	void Push(std::string name, MemType T::* member);
private:
	void Push(S_MemberDescriptor description);

	std::string_view m_Name;
	std::vector<S_MemberDescriptor> m_Members;
};

//=================================================================================
template<class T, class MemType>
void C_StructDescriptor::Push(std::string name, MemType T::* member)
{
	Push(S_MemberDescriptor(name, T_TypeToGLSL_v<MemType>));
}

//=================================================================================
template<class Derived>
class I_GLSLStruct
{
public:
	C_StructDescriptor GetDescription() const { return static_cast<const Derived*>(this)->GetDescriptionImpl(); };
	[[nodiscard]] std::string GetName() const { return static_cast<const Derived*>(this)->GetNameImpl(); };
};
}