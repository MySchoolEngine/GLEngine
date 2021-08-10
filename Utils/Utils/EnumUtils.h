#pragma once

#include <type_traits>

namespace Utils {
template <class E, typename = std::enable_if_t<std::is_enum_v<E>>> auto ToIndex(const E e)
{
	return static_cast<std::underlying_type_t<E>>(e);
}
} // namespace Utils
