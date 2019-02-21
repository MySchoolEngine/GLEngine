#pragma once

#include <Core/CoreMacros.h>

#include <Core/Logging/Data.h>
#include <Core/Logging/Logging.h>

namespace Core {
namespace Logging {

class C_Filter {
public:
	C_Filter(C_Filter* innerFilter = nullptr);
	virtual ~C_Filter();

	virtual bool Filter(const S_Data& message);
protected:
	/** ==============================================
	* @method:    FilterCheck
	* @return:    bool
	* @brief	   Returns true if message should be filtered out
	** ==============================================*/
	virtual bool FilterCheck(const S_Data& message) = 0;
	C_Filter* m_InnerFilter;
};

//=================================================================================
class C_ContextFilter : public C_Filter {
public:
	C_ContextFilter(E_Context context, C_Filter* innerFilter = nullptr);
	virtual ~C_ContextFilter() = default;
protected:
	virtual bool FilterCheck(const S_Data& message) override;

	E_Context m_Context;
};

//=================================================================================
class C_LevelFilter : public C_Filter {
public:
	C_LevelFilter(E_Level context, C_Filter* innerFilter = nullptr);
	virtual ~C_LevelFilter() = default;
protected:
	virtual bool FilterCheck(const S_Data& message) override;

	E_Level m_Level;
};

}
}

