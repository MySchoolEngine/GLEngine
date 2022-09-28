#pragma once

// function from C++20 that are already possible to implement in current standard
#ifndef __cpp_lib_remove_cvref
namespace std {
// https://en.cppreference.com/w/cpp/types/remove_cvref
template <class T> struct remove_cvref {
	typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template <class T> using remove_cvref_t = typename remove_cvref<T>::type;
}
#endif
