#include <Utils/Logging/LoggingMacros.h>
#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/GLMRTTI.h>
#include <Utils/Serialization/SerializationTraits.h>
#include <Utils/Serialization/XMLSerialize.h>

#include <Core/CoreMacros.h>

#include <rttr/type>
#include <sstream>

namespace GLEngine::Utils {
//=================================================================================
pugi::xml_document C_XMLSerializer::Serialize(const rttr::instance obj)
{
	if (!obj.is_valid())
		return {};

	pugi::xml_document doc;
	pugi::xml_node	   node	 = doc.append_child(GetNodeName(obj.get_type()).to_string().c_str());
	const auto		   node2 = SerializeObject(obj, node);
	// std::stringstream		ss;
	// pugi::xml_writer_stream writer(ss);
	// node2.print(writer);
	return doc;
}

//=================================================================================
pugi::xml_node C_XMLSerializer::SerializeObject(const rttr::instance& obj2, pugi::xml_node& node)
{
	using namespace ::Utils::Reflection;
	const rttr::instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;

	const auto prop_list = obj.get_derived_type().get_properties();
	for (auto prop : prop_list)
	{
		if (HasMetadataMember<SerializationCls::NoSerialize>(prop))
			continue;

		WriteProperty(prop, obj, node);
	}

	return node;
}

//=================================================================================
void C_XMLSerializer::WriteProperty(const rttr::property& prop, const rttr::instance& var, pugi::xml_node& parent)
{
	using namespace ::Utils::Reflection;
	auto type	   = prop.get_type();
	auto propValue = prop.get_value(var);
	if (HasMetadataMember<SerializationCls::DerefSerialize>(prop))
	{
		GLE_ASSERT(type.is_pointer(), "Cannot dereference {}", type);
		type = type.get_raw_type();
	}
	if (IsAtomicType(type))
	{
		WriteAtomics(type, propValue, parent.append_attribute(prop.get_name().to_string().c_str()));
	}
	else if (propValue.is_sequential_container())
	{
		const auto view = propValue.create_sequential_view();
		if (view.is_empty())
			return;
		auto node = parent.append_child(prop.get_name().to_string().c_str());
		WriteArray(view, node);
	}
	else if (propValue.is_associative_container())
	{
		const auto view = propValue.create_associative_view();
		if (view.is_empty())
			return;
		auto node = parent.append_child(prop.get_name().to_string().c_str());
		WriteAssociativeArray(view, node);
	}
	else
	{
		SerializeObject(propValue, parent.append_child(prop.get_name().to_string().c_str()));
	}
}

//=================================================================================
void C_XMLSerializer::WriteAtomics(const rttr::type& type, const rttr::variant& obj, pugi::xml_attribute& attr)
{
	if (obj.get_type().is_pointer())
	{
		if (type == rttr::type::get<bool>())
			attr.set_value(*obj.convert<bool*>());
		else if (type == rttr::type::get<char>())
			attr.set_value(*obj.convert<char*>());
		else if (type == rttr::type::get<int8_t>())
			attr.set_value(*obj.convert<int8_t*>());
		else if (type == rttr::type::get<int16_t>())
			attr.set_value(*obj.convert<int16_t*>());
		else if (type == rttr::type::get<int32_t>())
			attr.set_value(*obj.convert<int32_t*>());
		else if (type == rttr::type::get<int64_t>())
			attr.set_value(*obj.convert<int64_t*>());
		else if (type == rttr::type::get<uint8_t>())
			attr.set_value(*obj.convert<uint8_t*>());
		else if (type == rttr::type::get<uint16_t>())
			attr.set_value(*obj.convert<uint16_t*>());
		else if (type == rttr::type::get<uint32_t>())
			attr.set_value(*obj.convert<uint32_t*>());
		else if (type == rttr::type::get<uint64_t>())
			attr.set_value(*obj.convert<uint64_t*>());
		else if (type == rttr::type::get<float>())
			attr.set_value(*obj.convert<float*>());
		else if (type == rttr::type::get<double>())
			attr.set_value(*obj.convert<double*>());
	}
	else if (type.is_arithmetic())
	{
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
			attr.set_value(obj.to_float());
		else if (type == rttr::type::get<double>())
			attr.set_value(obj.to_double());
	}
	else if (type == rttr::type::get<std::string>())
		attr.set_value(obj.to_string().c_str());
	else if (type.is_enumeration())
	{
		bool	   ok	  = false;
		const auto result = obj.to_string(&ok);
		if (ok)
		{
			attr.set_value(obj.to_string().c_str());
		}
		else
		{
			ok		   = false;
			auto value = obj.to_uint64(&ok);
			if (ok)
				attr.set_value(value);
			// GLE_ASSERT(ok, "Wrong enum serialized");
		}
	}
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

//=================================================================================
void C_XMLSerializer::WriteAssociativeArray(const rttr::variant_associative_view& view, pugi::xml_node& parent)
{
	for (const auto& item : view)
	{
		auto itemNode = parent.append_child("item");
		{
			auto		  keyNode = itemNode.append_child("key");
			rttr::variant key	  = item.first;
			while (key.get_type().is_wrapper())
				key = key.extract_wrapped_value();
			const auto type = rttr::instance(key).get_derived_type();
			if (IsAtomicType(type))
			{
				WriteAtomics(type, key, keyNode.append_attribute("value"));
			}
			else
			{
				SerializeObject(key, keyNode.append_child(GetNodeName(type).to_string().c_str()));
			}
		}
		{
			auto		  valueNode = itemNode.append_child("value");
			rttr::variant value		= item.second;
			while (value.get_type().is_wrapper())
				value = value.extract_wrapped_value();
			const auto type = rttr::instance(value).get_derived_type();
			if (IsAtomicType(type))
			{
				WriteAtomics(type, value, valueNode.append_attribute("value"));
			}
			else
			{
				SerializeObject(value, valueNode.append_child(GetNodeName(type).to_string().c_str()));
			}
		}
	}
}

} // namespace GLEngine::Utils
