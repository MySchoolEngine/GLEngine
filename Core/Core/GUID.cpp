#include <CoreStdafx.h>

#include <Core/GUID.h>

RTTR_REGISTRATION
{
	rttr::registration::class_<GLEngine::GUID>("GUID")
		.constructor<>()(rttr::policy::ctor::as_object)
		.constructor<std::string_view>()
		.property("ID", &GLEngine::GUID::toString, &GLEngine::GUID::SetID);
}

namespace GLEngine {

//=================================================================================
GUID NextGUID()
{
	return xg::newGuid();
}

//=================================================================================
GUID::GUID(std::string_view fromString)
	: m_Guid(fromString)
{
}

//=================================================================================
GUID::GUID()
{
}

//=================================================================================
GUID::GUID(xg::Guid g)
	: m_Guid(g)
{
}

//=================================================================================
bool GUID::isValid() const
{
	return m_Guid.isValid();
}

//=================================================================================
std::string GUID::toString() const
{
	return m_Guid.str();
}

//=================================================================================
void GUID::SetID(std::string ID)
{
	m_Guid = xg::Guid(ID);
}

//=================================================================================
void GUID::swap(GUID& guid) noexcept
{
	m_Guid.swap(guid.m_Guid);
}

//=================================================================================
std::size_t GUID::hash() const
{
	return std::hash<xg::Guid>{}(m_Guid);
}

//=================================================================================
bool operator<(const GUID& lhs, const GUID& rhs)
{
	return lhs.m_Guid < rhs.m_Guid;
}

//=================================================================================
bool GUID::operator!=(const GUID& other) const
{
	return m_Guid.operator!=(other.m_Guid);
}

//=================================================================================
bool GUID::operator==(const GUID& other) const
{
	return m_Guid.operator==(other.m_Guid);
}

//=================================================================================
const GUID GUID::INVALID_GUID = xg::Guid();

//=================================================================================
GUID::operator std::string() const
{
	return m_Guid.operator std::string();
}

} // namespace GLEngine

namespace std {
template <> void swap(GLEngine::GUID& lhs, GLEngine::GUID& rhs) noexcept
{
	lhs.swap(rhs);
}
} // namespace std
