#include <Utils/Logging/Filter.h>

namespace Utils::Logging {

//=================================================================================
C_Filter::C_Filter(C_Filter* innerFilter)
	: m_InnerFilter(innerFilter)
{
}

//=================================================================================
C_Filter::~C_Filter()
{
	if (m_InnerFilter)
	{
		delete m_InnerFilter;
	}
}

//=================================================================================
bool C_Filter::Filter(const S_Data& message)
{
	if (FilterCheck(message))
	{
		return false;
	}
	if (m_InnerFilter)
	{
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
	if (message.m_Context == m_Context)
	{
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
	if (message.m_Level == m_Level)
	{
		return true;
	}
	return false;
}

//=================================================================================
// C_PassAllFilter
//=================================================================================
C_TextFilter::C_TextFilter(const std::string& text, C_Filter* innerFilter /*= nullptr*/)
	: C_Filter(innerFilter)
	, m_Text(text)
{
}

//=================================================================================
bool C_TextFilter::FilterCheck(const S_Data& message)
{
	return (message.m_Text.find(m_Text) == std::string::npos);
}

//=================================================================================
// C_PassAllFilter
//=================================================================================
bool C_PassAllFilter::FilterCheck(const S_Data& message)
{
	return false;
}

//=================================================================================
// C_NegationFilter
//=================================================================================
C_NegationFilter::C_NegationFilter(C_Filter* negatedFilter, C_Filter* inner)
	: C_Filter(inner)
	, m_NegatedFilter(negatedFilter)
{
}

//=================================================================================
C_NegationFilter::~C_NegationFilter()
{
	if (m_NegatedFilter)
	{
		delete m_NegatedFilter;
	}
}

//=================================================================================
bool C_NegationFilter::Filter(const S_Data& message)
{
	return !C_Filter::Filter(message);
}

//=================================================================================
bool C_NegationFilter::FilterCheck(const S_Data& message)
{
	return !m_NegatedFilter->Filter(message);
}

} // namespace Utils::Logging