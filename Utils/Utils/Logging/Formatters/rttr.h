#pragma once

#include <fmt/format.h>

#include <rttr/type>

template <> struct fmt::formatter<rttr::string_view> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext> auto format(const rttr::string_view& stringView, FormatContext& ctx) { return format_to(ctx.out(), "{}", stringView.data()); }
};

template <> struct fmt::formatter<rttr::type> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext> auto format(const rttr::type& type, FormatContext& ctx) { return format_to(ctx.out(), "rttr::type({})", type.get_name().data()); }
};
