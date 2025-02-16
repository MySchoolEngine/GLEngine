#pragma once

#include <filesystem>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <fmt/format.h>
#pragma warning(pop)

template <> struct fmt::formatter<std::filesystem::path> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext> auto format(const std::filesystem::path& filepath, FormatContext& ctx) { return format_to(ctx.out(), "({})", filepath.generic_string()); }
};
