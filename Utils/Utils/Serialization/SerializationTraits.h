#pragma once

#include <rttr/type>

#include <string>

namespace GLEngine::Utils {
[[nodiscard]] static inline bool IsAtomicType(const rttr::type& type)
{
	static const auto filesystemPathType = rttr::type::get<std::filesystem::path>();
	static const auto stringType		 = rttr::type::get<std::string>();
	return type.is_arithmetic() || type == stringType || type.is_enumeration() || type == filesystemPathType;
}

[[nodiscard]] static inline bool IsEmpty(const rttr::type& type, const rttr::variant& var)
{
	static const auto filesystemPathType = rttr::type::get<std::filesystem::path>();
	static const auto stringType		 = rttr::type::get<std::string>();
	if (type==filesystemPathType) {
		return var.get_value<std::filesystem::path>().empty();
	}
	if (type == stringType)
	{
		return var.get_value<std::string>().empty();
	}
	return false;
}
}

