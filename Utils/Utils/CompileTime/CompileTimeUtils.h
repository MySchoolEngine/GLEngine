#pragma once

#include <cstdint>
#include <type_traits>

// https://stackoverflow.com/a/47020053/3243840
constexpr int32_t basename_index(const char* const path, const int32_t index = 0, const int32_t slash_index = -1)
{
	return path[index] ? ((path[index] == '/' || path[index] == '\\') // (see below)
							  ? basename_index(path, index + 1, index)
							  : basename_index(path, index + 1, slash_index))
					   : (slash_index + 1);
}

template <int32_t Value> struct require_at_compile_time {
	static constexpr const int32_t value = Value;
};

// Solve random crashes in __FILENAME__ on GCC, temporary solution as I would like
// to migrate to the C++20 anyway
#ifdef __cpp_lib_source_location
	#include <source_location>
	#define __FILENAME__ std::source_location::current().file_name()
#else
	#define __FILENAME__ (__FILE__ + require_at_compile_time<basename_index(__FILE__)>::value)
#endif

//=================================================================================
struct T_IndexOfType {
	template <class T, class... Types> static constexpr int IndexOf() { return IndexImpl<T, Types...>(); }

private:
	template <class T, class U, class... Types> static constexpr int IndexImpl(int index = 0)
	{
		if constexpr (std::is_same_v<T, U>)
		{
			return index;
		}
		else if constexpr (sizeof...(Types) > 0)
		{
			return IndexImpl<T, Types...>(index + 1);
		}
		else
		{
			static_assert(sizeof...(Types) == 0, "Type not found inside a pack!");
		}
	}
};

template <class T, class... Types> struct T_ContainsType {
private:
	template <class T1, class U, class... Types1> static constexpr bool Contains()
	{
		if constexpr (std::is_same_v<T1, U>)
		{
			return true;
		}
		else if constexpr (sizeof...(Types1) > 0)
		{
			return Contains<T1, Types1...>();
		}
		else
		{
			return false;
		}
	}

public:
	constexpr static bool value = Contains<T, Types...>();
};

template <class T, class... Types> constexpr bool T_ContainsType_v = T_ContainsType<T, Types...>::value;

namespace Utils {

template <bool... values> struct all_of : std::true_type {};
template <bool... values> struct all_of<true, values...> : all_of<values...> {};
template <bool... values> struct all_of<false, values...> : std::false_type {};

template <bool... values> struct none_of : std::true_type {};
template <bool... values> struct none_of<false, values...> : none_of<values...> {};
template <bool... values> struct none_of<true, values...> : std::false_type {};

template <bool... values> struct any_of : std::false_type {};
template <bool... values> struct any_of<true, values...> : std::true_type {};
template <bool... values> struct any_of<false, values...> : any_of<values...> {};

template <bool... values> constexpr bool all_of_v  = all_of<values...>::value;
template <bool... values> constexpr bool none_of_v = none_of<values...>::value;
template <bool... values> constexpr bool any_of_v = any_of<values...>::value;
} // namespace Utils