#pragma once

#include <Utils/Allocation/Internal/AllocatorUtils.h>

#include <memory>

// technique from Andrei Alexandrescu: Modern C++ Design: Generic Programming
// and Design
namespace Utils::Allocation::Traits {

template <typename T> struct has_deallocate_all {
	template <typename U, void (U::*)()> struct Check;
	template <typename U> static constexpr bool test(Check<U, &U::deallocateAll>*) { return true; }
	template <typename U> static constexpr bool test(...) { return false; }

	static constexpr bool value = test<T>(nullptr);
};

template <typename T> struct has_owns {
	template <typename U, void (U::*)()> struct Check;
	template <typename U> static constexpr bool test(Check<U, &U::owns>*) { return true; }
	template <typename U> static constexpr bool test(...) { return false; }

	static constexpr bool value = test<T>(nullptr);
};
} // namespace Utils::Allocation::Traits