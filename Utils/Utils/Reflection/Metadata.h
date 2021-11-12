#pragma once

#include <rttr/type>
#include <type_traits>

namespace Utils::Reflection {

template <class Name> struct IsMetadataName : std::false_type {};
template <class Name> constexpr bool IsMetadataName_v = IsMetadataName<Name>::value;

template <class Type> struct ParentMetatype {};
template <class Type> using T_ParentMetatype = typename ParentMetatype<Type>::type;

template <auto Type> struct MemberType {};
template <auto Type> using MemberType_t = typename MemberType<Type>::type;

template <auto Type> struct MemberIsOptional : std::false_type {};
template <auto Type> constexpr bool MemberIsOptional_v = MemberIsOptional<Type>::value;

#define REGISTER_META_CLASS(cls, Parent) \
template <> struct ::Utils::Reflection::ParentMetatype<cls> { using type = Parent; }; \
template <> struct ::Utils::Reflection::IsMetadataName<cls> : std::true_type {}

#define REGISTER_META_MEMBER_TYPE(member, Type) \
template <> struct ::Utils::Reflection::MemberType<member> {using type = Type;};

//=================================================================================
// Getters
//=================================================================================
template <auto Member, class Enum = decltype(Member)>
MemberType_t<Member> GetMetadataMember(const rttr::property& prop)
{
	static_assert(IsMetadataName_v<Enum>, "Given member name must be registered meta member.");
	const auto metadata = prop.get_metadata(Member);
	if constexpr (!MemberIsOptional_v<Member>) {
		GLE_ASSERT(metadata.is_valid(), "Mandatory property metamember missing.");
	}
	return metadata.get_value<MemberType_t<Member>>();
}

//=================================================================================
template <class Enum>
rttr::variant GetMetadataMember(const rttr::property& prop, const Enum member)
{
	static_assert(IsMetadataName_v<Enum>, "Given member name must be registered meta member.");
	const auto metadata = prop.get_metadata(Member);
	GLE_ASSERT(MemberIsOptional_v<Member> || metadata.is_valid(), "Mandatory property metamember missing.");
	return metadata;
}

//=================================================================================
template <auto Class, class Enum = decltype(Class)>
[[nodiscard]] bool IsMetaclass(const rttr::property& prop)
{
	static_assert(std::is_enum_v<Enum>, "Class name could be only enum class.");
	static_assert(IsMetadataName_v<Enum>, "Given class name must be registered meta member.");
	return prop.get_metadata(Class).is_valid();
}

//=================================================================================
// Registration
//=================================================================================
template <auto Member, class Type, class Enum = decltype(Member)>
rttr::detail::metadata RegisterMetamember(const Type& value)
{
	static_assert(std::is_enum_v<Enum>, "Member name could be only enum class.");
	static_assert(IsMetadataName_v<Enum>, "Given member name must be registered meta member.");
	static_assert(std::is_convertible_v<Type, MemberType_t<Member>>, "Value must be convertible to registered type.");
	return rttr::detail::metadata(Member, value);
}

//=================================================================================
template <auto MetaClass, class Enum = decltype(MetaClass)> 
rttr::detail::metadata RegisterMetaclass()
{
	static_assert(std::is_enum_v<Enum>, "Class name could be only enum class.");
	static_assert(IsMetadataName_v<Enum>, "Given class name must be registered meta member.");
	return rttr::detail::metadata(MetaClass, 1);
}

//=================================================================================
// Definitions
//=================================================================================
enum class Metatype
{
  Serialization,
  GUI, // -> MetaGUI
};
template <> struct IsMetadataName<Metatype> : std::true_type {};

enum class MetaGUI
{
  Slider,
  Colour,
  Vec3,
};
REGISTER_META_CLASS(MetaGUI, Metatype);

namespace UI
{
template <MetaGUI Class> struct UIMetaclassToType {};
template <MetaGUI Class> using UIMetaclassToType_t = typename UIMetaclassToType<Class>::type;
template <> struct UIMetaclassToType<MetaGUI::Vec3> { using type = glm::vec3; };
template <> struct UIMetaclassToType<MetaGUI::Slider> { using type = float; };
//template <> struct UIMetaclassToType<MetaGUI::Colour> { using type = Colours::T_Colour; };
//=================================================================================
template <MetaGUI Class>
[[nodiscard]] bool IsUIMetaclass(const rttr::property& prop)
{
	const auto isRightClass = IsMetaclass<Class>(prop);
	if (isRightClass)
	{
		// Those are actually compile time problems. So I do not include it into the result as it should be checked before committing.
		GLE_ASSERT(prop.get_type().is_pointer(), "Property for UI needs to be rttr::policy::prop::bind_as_ptr in order to ImGui make work.");
		// unfortunately, it is impossible to check this during registration
		GLE_ASSERT(rttr::type::get<UIMetaclassToType_t<Class>*>() == prop.get_type(), "Property has wrong type");
	}
	return isRightClass;
}

enum class Slider
{
  Name,
  Min,
  Max,
};

enum class Checkbox
{
  Name,
};

enum class Vec3
{
  Name,
};
} // namespace UI
REGISTER_META_CLASS(UI::Slider, MetaGUI);
REGISTER_META_MEMBER_TYPE(UI::Slider::Name, std::string);
REGISTER_META_MEMBER_TYPE(UI::Slider::Min, float);
REGISTER_META_MEMBER_TYPE(UI::Slider::Max, float);

REGISTER_META_CLASS(UI::Checkbox, MetaGUI);
REGISTER_META_MEMBER_TYPE(UI::Checkbox::Name, std::string);

REGISTER_META_CLASS(UI::Vec3, MetaGUI);
REGISTER_META_MEMBER_TYPE(UI::Vec3::Name, std::string);

}