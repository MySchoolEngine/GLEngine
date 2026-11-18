#pragma once

#include <DULib/StrongType.h>

#include <rttr/type.h>
#include <rttr/wrapper_mapper.h>

// Generic RTTR glue for DULib::StrongType<Underlying, Tag> - works for any tag, not tied to any
// particular quantity. Include this wherever a StrongType is registered as an RTTR property.
namespace rttr::detail {
template <typename Underlying, typename Tag> struct template_type_trait<DULib::StrongType<Underlying, Tag>> : std::true_type {
	static std::vector<::rttr::type> get_template_arguments() { return {::rttr::type::get<Underlying>()}; }
};
} // namespace rttr::detail

namespace rttr {
// wrapped_type must be a true reference - RTTR takes the address of whatever get() returns to
// support in-place editing (see rttr::detail::wrapped_raw_addressof). Returning Underlying by
// value here would hand RTTR the address of a temporary.
template <typename Underlying, typename Tag> struct wrapper_mapper<DULib::StrongType<Underlying, Tag>> {
	using wrapped_type = Underlying&;
	using type		   = DULib::StrongType<Underlying, Tag>;

	inline static wrapped_type get(const type& obj) { return const_cast<Underlying&>(obj.get()); }
	inline static type			create(const wrapped_type& value) { return type(value); }
};
} // namespace rttr
