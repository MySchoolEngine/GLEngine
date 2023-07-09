#pragma once

#include <cstddef>

namespace Utils {
template<class T>
std::size_t HashCombine(std::size_t previous, const T& obj)
{
	return previous ^ std::hash<T>{}(obj) + 0x9e3779b9 + (previous << 6) + (previous >> 2);
}

template<class T>
std::size_t HashVector(std::size_t previous, const std::vector<T>& vec)
{
	auto seed = vec.size();
	for (const T& obj : vec) {
		seed = HashCombine(seed, obj);
	}
	return HashCombine(previous, seed);
}
}

