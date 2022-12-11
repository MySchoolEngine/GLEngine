#include <RendererStdafx.h>

#include <Renderer/Animation/Timestamp.h>

namespace GLEngine::Renderer {

//=================================================================================
// S_Timestamp
//=================================================================================
S_Timestamp::S_Timestamp(float timestamp)
	: m_Timestamp(S_Timestamp::Clamp(timestamp))
{
}

//=================================================================================
S_Timestamp::S_Timestamp()
	: m_Timestamp(0.f)
{
}

//=================================================================================
float S_Timestamp::Clamp(float f)
{
	return std::clamp(f, 0.0f, 1.0f);
}

//=================================================================================
bool S_Timestamp::operator>=(const S_Timestamp& other) const
{
	return m_Timestamp >= other.m_Timestamp;
}

//=================================================================================
bool S_Timestamp::operator>(const S_Timestamp& other) const
{
	return m_Timestamp > other.m_Timestamp;
}

//=================================================================================
bool S_Timestamp::operator<(const S_Timestamp& other) const
{
	return m_Timestamp < other.m_Timestamp;
}

//=================================================================================
bool S_Timestamp::operator<=(const S_Timestamp& other) const
{
	return m_Timestamp <= other.m_Timestamp;
}

//=================================================================================
S_Timestamp S_Timestamp::operator-(const S_Timestamp& rhs) const
{
	return S_Timestamp(m_Timestamp - rhs.m_Timestamp);
}

//=================================================================================
S_Timestamp S_Timestamp::operator+(const S_Timestamp& rhs) const
{
	return S_Timestamp(m_Timestamp + rhs.m_Timestamp);
}

} // namespace GLEngine::Renderer
