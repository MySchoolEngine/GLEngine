#pragma once

#include <glm/glm.hpp>

#include <fmt/format.h>

template <> struct fmt::formatter<glm::vec4> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext> auto format(const glm::vec4& vec, FormatContext& ctx) { return format_to(ctx.out(), "({}, {}, {}, {})", vec.x, vec.y, vec.z, vec.w); }
};

template <> struct fmt::formatter<glm::vec3> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext> auto format(const glm::vec3& vec, FormatContext& ctx) { return format_to(ctx.out(), "({}, {}, {})", vec.x, vec.y, vec.z); }
};

template <> struct fmt::formatter<glm::ivec2> : fmt::formatter<string_view> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext> auto format(const glm::ivec2& vec, FormatContext& ctx) { return format_to(ctx.out(), "({}, {})", vec.x, vec.y); }
};
