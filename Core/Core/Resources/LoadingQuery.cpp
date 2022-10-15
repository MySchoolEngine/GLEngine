#include <CoreStdafx.h>

#include <Core/Resources/LoadingQuery.h>


namespace GLEngine::Core {

//=================================================================================
void LoadingQuery::AddHandle(ResourceHandleBase handle)
{
	// only add such hanldes, that are not loaded yet
	// can also avoid dead locks in some situations
	if (handle.IsLoading())
		m_handles.push_back(handle);}

//=================================================================================
bool LoadingQuery::IsDone() const
{
	return std::all_of(m_handles.begin(), m_handles.end(), [](auto& handle) { return handle.IsLoading() == false; });
}

}