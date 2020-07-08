#pragma once

#include <fmt/format.h>

template <>
struct fmt::formatter<std::filesystem::path> : fmt::formatter<string_view> {
	template <typename FormatContext>
	auto format(const std::filesystem::path& filepath, FormatContext& ctx) {
		return format_to(
			ctx.out(),
			"({})",
			filepath.generic_string());
	}
};
