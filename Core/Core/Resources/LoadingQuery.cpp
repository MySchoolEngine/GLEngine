#include <CoreStdafx.h>

#include <Core/Resources/LoadingQuery.h>


namespace GLEngine::Core {

//=================================================================================
void LoadingQuery::AddHandle(const ResourceHandleBase& handle)
{
	// only add such handles, that are not loaded yet
	// can also avoid deadlocks in some situations
	if (handle.IsLoading())
		m_Handles.push_back(handle);}

//=================================================================================
bool LoadingQuery::IsDone() const
{
	return std::all_of(m_Handles.begin(), m_Handles.end(), [](auto& handle) { return handle.IsLoading() == false; });
}

}