#pragma once

//https://stackoverflow.com/a/47020053/3243840
constexpr int32_t basename_index(
	const char * const path, const int32_t index = 0, const int32_t slash_index = -1
)
{
	return path[index]
		? ((path[index] == '/' || path[index] == '\\')  // (see below)
			? basename_index(path, index + 1, index)
			: basename_index(path, index + 1, slash_index)
			)
		: (slash_index + 1)
		;
}

template <int32_t Value>
struct require_at_compile_time
{
	static constexpr const int32_t value = Value;
};

#define __FILENAME__ (__FILE__ + require_at_compile_time<basename_index(__FILE__)>::value)