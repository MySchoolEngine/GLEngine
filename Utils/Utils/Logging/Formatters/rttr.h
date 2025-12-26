#pragma once
#include <Core/CoreMacros.h>

GL_PUSH_WARNINGS()
#pragma warning(disable : 4996)
#include <fmt/format.h>
#ifdef __GNUC__
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <rttr/type>
GL_POP_WARNINGS()


template <> struct fmt::formatter<rttr::string_view> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext> auto format(const rttr::string_view& stringView, FormatContext& ctx) { return format_to(ctx.out(), "{}", stringView.data()); }
};

template <> struct fmt::formatter<rttr::type> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext> auto format(const rttr::type& type, FormatContext& ctx) { return format_to(ctx.out(), "rttr::type({})", type.get_name().data()); }
};

template <> struct fmt::formatter<rttr::property> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext> auto format(const rttr::property& property, FormatContext& ctx)
	{
		return format_to(ctx.out(), "rttr::property({} {})", property.get_type(), property.get_name());
	}
};