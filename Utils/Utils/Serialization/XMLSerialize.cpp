#include <Utils/Serialization/GLMRTTI.h>
#include <Utils/Serialization/XMLSerialize.h>

#include <rttr/type>
#include <sstream>

namespace GLEngine::Utils {
//=================================================================================
pugi::xml_document C_XMLSerializer::Serialize(const rttr::instance obj)
{
	if (!obj.is_valid())
		return {};

	pugi::xml_document		doc;
	pugi::xml_node			node  = doc.append_child(GetNodeName(obj.get_type()).to_string().c_str());
	const auto				node2 = SerializeObject(obj, node);
	// std::stringstream		ss;
	// pugi::xml_writer_stream writer(ss);
	// node2.print(writer);
	return doc;
}

//=================================================================================
pugi::xml_node C_XMLSerializer::SerializeObject(const rttr::instance& obj2, pugi::xml_node& node)
{
	const rttr::instance obj   = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;

	const auto prop_list = obj.get_derived_type().get_properties();
	for (auto prop : prop_list)
	{
		// todo
		if (prop.get_metadata("NO_SERIALIZE"))
			continue;

		WriteProperty(prop, obj, node);
	}

	return node;
}

//=================================================================================
void C_XMLSerializer::WriteProperty(const rttr::property& prop, const rttr::instance& var, pugi::xml_node& parent)
{
	const auto type		 = prop.get_type();
	const auto propValue = prop.get_value(var);
	if (type.is_arithmetic() || type == rttr::type::get<std::string>())
	{
		WriteAtomics(prop, propValue, parent);
	}
	else if (propValue.is_sequential_container())
	{
		auto node = parent.append_child(prop.get_name().to_string().c_str());
		WriteArray(propValue.create_sequential_view(), node);
	}
	else
	{
		SerializeObject(propValue, parent.append_child(prop.get_name().to_string().c_str()));
	}
}

//=================================================================================
void C_XMLSerializer::WriteAtomics(const rttr::property& prop, const rttr::variant& obj, pugi::xml_node& node)
{
	const auto type = prop.get_type();
	const auto name = prop.get_name();
	auto	   attr = node.append_attribute(name.to_string().c_str());

	if (type == rttr::type::get<bool>())
		attr.set_value(obj.to_bool());
	else if (type == rttr::type::get<char>())
		attr.set_value(obj.to_bool());
	else if (type == rttr::type::get<int8_t>())
		attr.set_value(obj.to_int8());
	else if (type == rttr::type::get<int16_t>())
		attr.set_value(obj.to_int16());
	else if (type == rttr::type::get<int32_t>())
		attr.set_value(obj.to_int32());
	else if (type == rttr::type::get<int64_t>())
		attr.set_value(obj.to_int64());
	else if (type == rttr::type::get<uint8_t>())
		attr.set_value(obj.to_uint8());
	else if (type == rttr::type::get<uint16_t>())
		attr.set_value(obj.to_uint16());
	else if (type == rttr::type::get<uint32_t>())
		attr.set_value(obj.to_uint32());
	else if (type == rttr::type::get<uint64_t>())
		attr.set_value(obj.to_uint64());
	else if (type == rttr::type::get<float>())
		attr.set_value(obj.to_double());
	else if (type == rttr::type::get<double>())
		attr.set_value(obj.to_double());
	else if (type == rttr::type::get<std::string>())
		attr.set_value(obj.to_string().c_str());
}

//=================================================================================
void C_XMLSerializer::WriteArray(const rttr::variant_sequential_view& view, pugi::xml_node& parent)
{
	for (const auto& item : view)
	{
		const rttr::instance obj  = item.extract_wrapped_value();
		const rttr::instance ins  = obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj;
		auto				 node = parent.append_child(GetNodeName(ins.get_derived_type()).to_string().c_str());
		SerializeObject(ins, node);
	}
}

//=================================================================================
rttr::string_view C_XMLSerializer::GetNodeName(const rttr::type& type)
{
	rttr::type rawType = type;
	if (rawType.is_pointer())
	{
		rawType = rawType.get_raw_type();
	}
	if (rawType.is_wrapper())
	{
		rawType = rawType.get_wrapped_type();
	}
	if (rawType.is_pointer())
		rawType = rawType.get_raw_type();

	return rawType.get_name();
}

} // namespace GLEngine::Utils
