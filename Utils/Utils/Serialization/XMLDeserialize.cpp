#include <Utils/Serialization/GLMRTTI.h>
#include <Utils/Serialization/XMLDeserialize.h>

#include <rttr/type>

#include <Utils/Logging/LoggingMacros.h>

namespace GLEngine::Utils {

//=================================================================================
rttr::variant C_XMLDeserializer::DeserializeDoc(const pugi::xml_document& document)
{
	const auto rootNode = document.root().first_child();
	return DeserializeNode(rootNode, rttr::type::get_by_name(rootNode.name()));
}

//=================================================================================
rttr::variant C_XMLDeserializer::DeserializeNode(const pugi::xml_node& node, const rttr::type& type)
{
	if (type.is_valid() == false)
		return {};
	auto var = type.create();
	if (var.is_valid() == false)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Invalid variant created. Probably needs ctor to be registered.");
		return {};
	}
	CORE_LOG(E_Level::Error, E_Context::Core, "{}", var.get_type());
	for (auto& prop : type.get_properties()) {
		DeserializeProperty(prop, var, node);
	}

	return var;
}

//=================================================================================
void C_XMLDeserializer::DeserializeProperty(const rttr::property& prop, rttr::variant& owner, const pugi::xml_node& node)
{
	const auto type = prop.get_type();
	if (type.is_arithmetic() || type == rttr::type::get<std::string>())
	{
		prop.set_value(owner, DeserializeAtomic(node, prop));
	}
	else
	{
		const auto child	= node.child(prop.get_name().to_string().c_str());
		if (child)
			prop.set_value(owner, DeserializeNode(child, prop.get_type()));
	}
}

//=================================================================================
rttr::variant C_XMLDeserializer::DeserializeAtomic(const pugi::xml_node& node, const rttr::property& prop)
{
	const auto attr = node.attribute(prop.get_name().to_string().c_str());
	const auto type = prop.get_type();
	if (attr)
	{
		if (type == rttr::type::get<bool>())
			return attr.as_bool();
		else if (type == rttr::type::get<char>())
			return attr.as_bool();
		else if (type == rttr::type::get<int8_t>())
			return attr.as_int();
		else if (type == rttr::type::get<int16_t>())
			return attr.as_int();
		else if (type == rttr::type::get<int32_t>())
			return attr.as_int();
		else if (type == rttr::type::get<int64_t>())
			return attr.as_llong();
		else if (type == rttr::type::get<uint8_t>())
			return attr.as_uint();
		else if (type == rttr::type::get<uint16_t>())
			return attr.as_uint();
		else if (type == rttr::type::get<uint32_t>())
			return attr.as_uint();
		else if (type == rttr::type::get<uint64_t>())
			return attr.as_ullong();
		else if (type == rttr::type::get<float>())
			return attr.as_float();
		else if (type == rttr::type::get<double>())
			return attr.as_double();
		else if (type == rttr::type::get<std::string>())
			return attr.as_string();
	}
	CORE_LOG(E_Level::Error, E_Context::Core, "Unknown atomics.");
	return {};
}

}
