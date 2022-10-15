#include <CoreStdafx.h>

#include <Core/Resources/Resource.h>

namespace GLEngine::Core {

//=================================================================================
Resource::Resource() = default;

//=================================================================================
Resource::~Resource() = default;

//=================================================================================
bool Resource::IsReady() const
{
	return m_State == ResourceState::Ready;
}

//=================================================================================
ResourceState Resource::GetState() const
{
	return m_State;
}

} // namespace GLEngine::Core