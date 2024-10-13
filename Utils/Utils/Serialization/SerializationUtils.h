#pragma once

#include <string>
#include <type_traits>


template <class T> bool DeserializeString(const std::string& str, std::reference_wrapper<T> vec)
{
	return DeserializeString(str, vec.get());
}

template <class T> bool SerializeString(T vec, std::reference_wrapper<std::string> ret)
{
	return SerializeString(vec, ret.get());
}

template <class T> bool SerializeString(std::reference_wrapper<T> vec, std::reference_wrapper<std::string> ret)
{
	return SerializeString(vec.get(), ret.get());
}

#define REGISTER_SERIALIZATION(type)                                                                                                                                               \
	rttr::registration::method("DeserializeString", rttr::select_overload<bool(const std::string&, type&)>(&DeserializeString));                                                   \
	rttr::registration::method("DeserializeString", rttr::select_overload<bool(const std::string&, std::reference_wrapper<type>)>(&DeserializeString));                            \
	rttr::registration::method("SerializeString", rttr::select_overload<bool(const type&, std::reference_wrapper<std::string>)>(&SerializeString));                                \
	rttr::registration::method("SerializeString", rttr::select_overload<bool(const std::reference_wrapper<type>, std::reference_wrapper<std::string>)>(&SerializeString))

#define REGISTER_DEFAULT_VALUE(value) rttr::metadata("SerializationDefaultValue", value)