#pragma once

#include <GLRenderer/Helpers/GLSLTypeHelpers.h>

namespace GLEngine::GLRenderer::Shaders {

//=================================================================================
class S_MemberDescriptor {
public:
	constexpr S_MemberDescriptor(const std::string_view name, const std::string_view type)
		: m_Name(name)
		, m_Type(type)
	{
	}

	[[nodiscard]] std::string Generate() const;

	std::string_view m_Name;
	std::string_view m_Type;
};

//=================================================================================
class C_StructDescriptor {
public:
	explicit C_StructDescriptor(std::string_view name);
	[[nodiscard]] std::string Generate() const;

	template <class T, class MemType> void Push(std::string_view name, MemType T::*member);

private:
	void Push(S_MemberDescriptor description);

	std::string_view				m_Name;
	std::vector<S_MemberDescriptor> m_Members;
};

//=================================================================================
template <class T, class MemType> void C_StructDescriptor::Push(const std::string_view name, MemType T::*member)
{
	Push(S_MemberDescriptor(name, T_TypeToGLSL_v<MemType>));
}

//=================================================================================
template <class Derived> class I_GLSLStruct {
public:
	C_StructDescriptor		  GetDescription() const { return static_cast<const Derived*>(this)->GetDescriptionImpl(); };
	[[nodiscard]] std::string GetName() const { return static_cast<const Derived*>(this)->GetNameImpl(); };
};
} // namespace GLEngine::GLRenderer::Shaders