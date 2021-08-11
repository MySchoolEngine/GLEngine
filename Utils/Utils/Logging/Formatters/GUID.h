#pragma once

#include <Core/GUID.h>

#include <fmt/format.h>

template <> struct fmt::formatter<GLEngine::GUID> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext> auto format(const GLEngine::GUID& guid, FormatContext& ctx) { return format_to(ctx.out(), "({})", guid.toString()); }
};

