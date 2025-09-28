#include <Utils/Logging/LoggingMacros.h>
#include <Utils/Reflection/Metadata.h>
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

	pugi::xml_document	 doc;
	const pugi::xml_node node = doc.append_child(GetNodeName(obj.get_type()).to_string().c_str());
	SerializeObject(obj, node);
	return doc;
}

//=================================================================================
pugi::xml_node C_XMLSerializer::SerializeObject(const rttr::instance& obj2, const pugi::xml_node node)
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
void C_XMLSerializer::WriteProperty(const rttr::property& prop, const rttr::instance& var, pugi::xml_node parent)
{
	using namespace ::Utils::Reflection;
	rttr::type	  type		= prop.get_type();
	rttr::variant propValue = prop.get_value(var);
	if (type.is_wrapper())
		type = type.get_wrapped_type();

	if (auto defaultValue = prop.get_metadata("SerializationDefaultValue"))
	{
		auto defaultValueType = defaultValue.get_type();
		GLE_ASSERT(defaultValueType == type, "Type of default value doesn't match property type {}", prop);
		if (defaultValue == propValue)
		{
			return; // skip
		}
	}

	if (IsAtomicType(type))
	{
		if (IsEmpty(type, propValue) == false)
			WriteAtomics(type, propValue, parent.append_attribute(prop.get_name().to_string().c_str()));
	}
	else if (auto serializeStringFunction = rttr::type::get_global_method("SerializeString", {prop.get_type(), rttr::type::get<std::reference_wrapper<std::string>>()}))
	{
		std::string	  stringRet;
		rttr::variant stringVar = std::ref(stringRet);
		if (type.get_raw_type().is_sequential_container())
		{
			const auto view = propValue.create_sequential_view();
			if (view.is_empty())
				return;

			pugi::xml_node seqContainerNode = parent.append_child(prop.get_name().to_string().c_str());

			const rttr::variant returnValue = serializeStringFunction.invoke({}, propValue, stringVar);
			GLE_ASSERT(returnValue.is_valid(), "Wrong type for SerializeString");
			seqContainerNode.text().set(stringRet.c_str());
		}
		else
		{
			pugi::xml_attribute attr = parent.append_attribute(prop.get_name().to_string().c_str());

			const rttr::variant returnValue = serializeStringFunction.invoke({}, propValue, stringVar);
			GLE_ASSERT(returnValue.is_valid() && returnValue.is_type<bool>() && returnValue.get_value<bool>() == true, "Wrong type for SerializeString");
			attr.set_value(stringRet.c_str());
		}
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
		auto propNode = parent.append_child(prop.get_name().to_string().c_str());
		const auto type		= prop.get_type();
		if (type.get_raw_type().is_wrapper())
		{
			auto value = propValue;
			while (value.get_type().is_wrapper())
				value = value.extract_wrapped_value();

			auto propType = prop.get_type();
			while (propType.is_wrapper())
				propType = propType.get_wrapped_type();

			if (propType.is_pointer())
				propType = propType.get_raw_type();

			auto typeWrapped = rttr::instance(value).get_derived_type();
			if (typeWrapped.is_derived_from(propType) && typeWrapped != propType)
				propNode.append_attribute("derivedTypeCast").set_value(typeWrapped.get_name().data());
		}
		SerializeObject(propValue, propNode);
	}
}

//=================================================================================
void C_XMLSerializer::WriteAtomics(const rttr::type& type, const rttr::variant& obj, pugi::xml_attribute attr)
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
	else if (type == rttr::type::get<std::filesystem::path>())
		attr.set_value(obj.to_string().c_str());
	else if (type.is_enumeration())
	{
		bool	   ok	  = false;
		const auto result = obj.to_string(&ok);
		if (ok)
		{
			attr.set_value(result.c_str());
		}
		else
		{
			ok				 = false;
			const auto value = obj.to_uint64(&ok);
			if (ok)
				attr.set_value(value);
			// GLE_ASSERT(ok, "Wrong enum serialized");
		}
	}
}

//=================================================================================
void C_XMLSerializer::WriteArray(const rttr::variant_sequential_view& view, pugi::xml_node parent)
{
	for (const auto& item : view)
	{
		rttr::variant		obj	 = item.extract_wrapped_value();
		const rttr::variant ins	 = obj.get_type().get_raw_type().is_wrapper() ? obj.extract_wrapped_value() : obj;
		rttr::type			type = ins.get_type();

		if (rttr::instance(ins).get_type().get_raw_type().get_derived_classes().empty() == false)
			type = rttr::instance(ins).get_derived_type();
		GLE_ASSERT(ins.is_valid(), "invalid");
		auto node = parent.append_child(GetNodeName(type).to_string().c_str());
		if (IsAtomicType(type))
		{
			WriteAtomics(type, obj, node.append_attribute(GetNodeName(type).to_string().c_str()));
		}
		else
		{
			SerializeObject(obj, node);
		}
	}
}

//=================================================================================
rttr::string_view C_XMLSerializer::GetNodeName(const rttr::type& type)
{
	rttr::type rawType = type;
	while (rawType.is_pointer() || rawType.is_wrapper())
	{
		if (rawType.is_pointer())
		{
			rawType = rawType.get_raw_type();
		}
		if (rawType.is_wrapper())
		{
			rawType = rawType.get_wrapped_type();
		}
	}

	return rawType.get_name();
}

//=================================================================================
void C_XMLSerializer::WriteAssociativeArray(const rttr::variant_associative_view& view, pugi::xml_node parent)
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
