#pragma once

#include <Core/CoreMacros.h>

#include <Utils/Logging/Data.h>
#include <Utils/Logging/Logging.h>

namespace Utils {
namespace Logging {

/** ==============================================
 * @class C_Filter
 *
 * @brief	Should be written in that way, that it returns
 *			true for given set of messages.
 *
 * @author 	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * @date 	2019/09/08
 ** ==============================================*/
class API_EXPORT C_Filter {
public:
	C_Filter(C_Filter* innerFilter = nullptr);
	virtual ~C_Filter();
	
	/**
	 * @return: bool - true if message passed
	 **/
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
class API_EXPORT C_ContextFilter : public C_Filter {
public:
	C_ContextFilter(E_Context context, C_Filter* innerFilter = nullptr);
	virtual ~C_ContextFilter() = default;
protected:
	virtual bool FilterCheck(const S_Data& message) override;

	E_Context m_Context;
};

//=================================================================================
class API_EXPORT C_LevelFilter : public C_Filter {
public:
	C_LevelFilter(E_Level context, C_Filter* innerFilter = nullptr);
	virtual ~C_LevelFilter() = default;
protected:
	virtual bool FilterCheck(const S_Data& message) override;

	E_Level m_Level;
};

//=================================================================================
class API_EXPORT C_PassAllFilter : public C_Filter {
public:
	virtual ~C_PassAllFilter() = default;
protected:
	virtual bool FilterCheck(const S_Data& message) override;
};

//=================================================================================
class API_EXPORT C_NegationFilter : public C_Filter {
public:
	C_NegationFilter(C_Filter* negatedFilter, C_Filter* inner = nullptr);
	virtual ~C_NegationFilter();
	virtual bool Filter(const S_Data& message) override;
private:
	virtual bool FilterCheck(const S_Data& message) override;
	C_Filter* m_NegatedFilter;
};

}
}

