#pragma once

#include <rttr/type>

#include <string>

namespace GLEngine::Utils {
[[nodiscard]] static inline bool IsAtomicType(const rttr::type& type)
{
	return type.is_arithmetic() || type == rttr::type::get<std::string>() || type.is_enumeration();
}
}

