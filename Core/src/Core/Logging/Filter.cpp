#include <CoreStdafx.h>

#include <Core/Logging/Filter.h>



namespace Core {
namespace Logging {

//=================================================================================
C_Filter::C_Filter(C_Filter* innerFilter)
	: m_InnerFilter(innerFilter)
{

}

//=================================================================================
C_Filter::~C_Filter()
{
	if (m_InnerFilter) {
		delete m_InnerFilter;
	}
}

//=================================================================================
bool C_Filter::Filter(const S_Data& message)
{
	if (FilterCheck(message)) {
		return false;
	}
	if (m_InnerFilter) {
		return m_InnerFilter->Filter(message);
	}

	return true;
}

//=================================================================================
// C_ContextFilter
//=================================================================================
C_ContextFilter::C_ContextFilter(E_Context context, C_Filter* innerFilter /*= nullptr*/)
	: C_Filter(innerFilter)
	, m_Context(context)
{

}

//=================================================================================
bool C_ContextFilter::FilterCheck(const S_Data& message)
{
	if (message.m_Context == m_Context) {
		return true;
	}
	return false;
}

//=================================================================================
// C_LevelFilter
//=================================================================================
C_LevelFilter::C_LevelFilter(E_Level Level, C_Filter* innerFilter /*= nullptr*/)
	: C_Filter(innerFilter)
	, m_Level(Level)
{

}

//=================================================================================
bool C_LevelFilter::FilterCheck(const S_Data& message)
{
	if (message.m_Level == m_Level) {
		return true;
	}
	return false;
}

}
}
