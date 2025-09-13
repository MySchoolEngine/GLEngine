#include <Utils/Logging/LoggingMacros.h>
#include <Utils/Reflection/Metadata.h>
#include <Utils/STLAfx.h>
#include <Utils/Serialization/SerializationTraits.h>
#include <Utils/Serialization/XMLDeserialize.h>

#include <Core/CoreMacros.h>

#include <pugixml.hpp>

#include <rttr/type>

namespace GLEngine::Utils {

//=================================================================================
C_XMLDeserializer::C_XMLDeserializer(Core::C_ResourceManager& resMng)
	: m_Ctx({resMng})
{
}

//=================================================================================
rttr::variant C_XMLDeserializer::DeserializeDoc(const pugi::xml_document& document)
{
	const auto rootNode = document.root().first_child();
	const auto type		= rttr::type::get_by_name(rootNode.name());
	if (type.is_valid() == false)
		return {};
	auto var = type.create();
	if (var.is_valid() == false)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Invalid variant created. Probably needs ctor to be registered. Type = {}", type);
		return {};
	}
	DeserializeNode(rootNode, var);
	FinishDeserialization(type, var);
	return var;
}

//=================================================================================
rttr::variant C_XMLDeserializer::DeserializeNode(const pugi::xml_node& node, rttr::variant& var)
{
	using namespace ::Utils::Reflection;
	const rttr::instance var2 = var.get_type().get_raw_type().is_wrapper() ? rttr::instance(var).get_wrapped_instance() : rttr::instance(var);
	// CORE_LOG(E_Level::Error, E_Context::Core, "{}", var2.get_type());

	for (auto& prop : var2.get_type().get_properties())
	{
		if (HasMetadataMember<SerializationCls::NoSerialize>(prop))
			continue;

		DeserializeProperty(prop, var, node);
	}

	return var;
}

//=================================================================================
void C_XMLDeserializer::DeserializeProperty(const rttr::property& prop, rttr::variant& owner, const pugi::xml_node& node)
{
	auto type = prop.get_type();
	using namespace ::Utils::Reflection;

	if (auto deserializeStringFunction = rttr::type::get_global_method("DeserializeString", {rttr::type::get<const std::string&>(), type}))
	{
		const auto propertyName = prop.get_name().to_string();
		auto	   inlineType	= type;
		if (inlineType.is_wrapper())
			inlineType = inlineType.get_wrapped_type();
		if (inlineType.get_raw_type().is_sequential_container())
		{
			if (const auto seqContainerNode = node.child(propertyName.c_str()))
			{
				auto var  = prop.get_value(owner);
				auto view = var.create_sequential_view();
				deserializeStringFunction.invoke({}, std::string(seqContainerNode.text().as_string()), var);

				return;
			}
			else if (HasMetadataMember<SerializationCls::MandatoryProperty>(prop))
			{
				CORE_LOG(E_Level::Warning, E_Context::Core, "Missing attribute for property {}", prop);
			}
		}
		else
		{
			if (const auto attribute = node.attribute(propertyName.c_str()))
			{
				auto ownerType = owner.get_type();
				auto				var			 = prop.get_value(owner);
				var.convert(prop.get_type());
				const rttr::variant return_value = deserializeStringFunction.invoke({}, std::string(attribute.as_string()), var);
				if (!return_value.is_valid() || !return_value.is_type<bool>() || return_value.get_value<bool>() != true)
				{
					CORE_LOG(E_Level::Error, E_Context::Core, "Wrong format for property {}.", prop);
					return;
				}
				if (!prop.set_value(owner, var))
				{
					GLE_ASSERT(false, "Wrong type used {}", var.get_type());
				}
				return;
			}
			else if (HasMetadataMember<SerializationCls::MandatoryProperty>(prop))
			{
				CORE_LOG(E_Level::Warning, E_Context::Core, "Missing attribute for property {}", prop);
			}
		}
		// this gives second chance for such properties that have been saved before
		// the introduction of this option
	}

	if (type.is_wrapper())
		type = type.get_wrapped_type();

	if (IsAtomicType(type))
	{
		const auto propertyName = prop.get_name().to_string();
		if (const auto attribute = node.attribute(propertyName.c_str()))
		{
			rttr::variant var	   = prop.get_value(owner);
			DeserializeAtomic(attribute, type, var);
			const bool result = prop.set_value(owner, var);
			GLE_ASSERT(result, "Cannot set property {} to the type {}", propertyName, owner.get_type());
		}
		else
		{
			// support old attributes as elements
			if (const auto childNode = node.child(propertyName.c_str()))
			{
				CORE_LOG(E_Level::Warning, E_Context::Core, "Deprecated way to serialize atomic values as elements. Will be saved in the new way {}.", prop);
				if (const auto attribute = childNode.attribute(propertyName.c_str()))
				{
					rttr::variant var = prop.get_value(owner);
					DeserializeAtomic(attribute, type, var);
					const bool result = prop.set_value(owner, var);
					GLE_ASSERT(result, "Cannot set property {} to the type {}", propertyName, owner.get_type());
				}
				else
				{
					if (HasMetadataMember<SerializationCls::MandatoryProperty>(prop))
						CORE_LOG(E_Level::Warning, E_Context::Core, "Missing attribute for property {}.", prop);
				}
			}
			else
			{
				if (HasMetadataMember<SerializationCls::MandatoryProperty>(prop))
					CORE_LOG(E_Level::Warning, E_Context::Core, "Missing attribute for property {}.", prop);
			}
		}
	}
	else if (type.get_raw_type().is_sequential_container())
	{
		const auto child = node.child(prop.get_name().to_string().c_str());
		auto	   var	 = prop.get_value(owner);
		auto	   view	 = var.create_sequential_view();
		DeserializeArray(child, view);
		const bool result = prop.set_value(owner, var);
		GLE_ASSERT(result, "Cannot set property {} to the type {}", prop.get_name(), owner.get_type());
	}
	else if (type.get_raw_type().is_associative_container())
	{
		const auto child = node.child(prop.get_name().to_string().c_str());
		auto	   var	 = prop.get_value(owner);
		auto	   view	 = var.create_associative_view();
		DeserializeAssociativeArray(child, view);
		const bool result = prop.set_value(owner, var);
		GLE_ASSERT(result, "Cannot set property {} to the type {}", prop.get_name(), owner.get_type());
	}
	else
	{
		const auto child = node.child(prop.get_name().to_string().c_str());
		if (child)
		{
			auto var = prop.get_value(owner);
			if (!child.attribute("derivedTypeCast").empty())
			{
				auto typeCast = rttr::type::get_by_name(child.attribute("derivedTypeCast").as_string());
				GLE_ASSERT(typeCast, "Type {} does not exists", child.attribute("derivedTypeCast").as_string());
				var = typeCast.create();
				GLE_ASSERT(var.is_valid(), "Type {} cannot be instantiated.", type);
				auto varNode = DeserializeNode(child, var);
				FinishDeserialization(type, varNode);
				GLE_ASSERT(varNode.convert(prop.get_type()), "Cannot convert");//todo: Does this work on release?
				GLE_ASSERT(prop.set_value(owner, varNode), "Cannot assign property {}", prop);//todo: Does this work on release?
			}
			else
			{
				auto varNode = DeserializeNode(child, var);
				FinishDeserialization(type, varNode);
				const bool result = prop.set_value(owner, varNode);
				GLE_ASSERT(result, "Cannot set property {} to the type {}", prop.get_name().to_string(), owner.get_type());
			}
		}
	}
}

//=================================================================================
void C_XMLDeserializer::DeserializeArray(const pugi::xml_node& child, rttr::variant_sequential_view& view)
{
	int index = 0;
	for (const auto childNode : child)
	{
		const auto type = rttr::type::get_by_name(childNode.name());
		if (type.is_valid() == false)
			continue;

		rttr::variant var;
		if (IsAtomicType(type))
		{
			DeserializeAtomic(childNode.attribute(childNode.name()), type, var);
			GLE_ASSERT(var.convert(view.get_value_type()), "Cannot convert variable {} to {}", var.get_type(), view.get_value_type());
			GLE_ASSERT(var.is_valid(), "Cannot create {} var", type);
			const auto it = view.insert(view.end(), var);
			view.set_value(index, var);
		}
		else
		{
			var = type.create();
			GLE_ASSERT(var.is_valid(), "Cannot create {} var", type);
			DeserializeNode(childNode, var);
			GLE_ASSERT(var.convert(view.get_value_type()), "Cannot convert variable {} to {}", var.get_type(), view.get_value_type());
			GLE_ASSERT(var.is_valid(), "Cannot create {} var", type);

			const auto it = view.insert(view.end(), var);
			if (it == view.end())
			{
				CORE_LOG(E_Level::Error, E_Context::Core, "Failed to insert item {} into the array.", type);
			}
		}
		FinishDeserialization(type, var);
		index++;
	}
}

//=================================================================================
void C_XMLDeserializer::DeserializeAssociativeArray(const pugi::xml_node& child, rttr::variant_associative_view& view)
{
	const auto keyType	 = view.get_key_type();
	const auto valueType = view.get_value_type();
	for (const auto childNode : child)
	{
		if (childNode.name() != std::string_view("item"))
		{
			CORE_LOG(E_Level::Error, E_Context::Core, "Unexpected node in associative array. {}.", childNode.name());
			continue;
		}
		rttr::variant keyVar;
		rttr::variant valueVar;
		if (IsAtomicType(keyType))
		{
			keyVar = DeserializeAtomic(childNode.child("key").attribute("value"), keyType);
			if (!keyVar.is_valid())
				CORE_LOG(E_Level::Error, E_Context::Core, "Failed to deserialize atomic type key.");
		}
		else
			CORE_LOG(E_Level::Error, E_Context::Core, "Well, not implemented. Yet?");

		if (IsAtomicType(valueType))
		{
			valueVar = DeserializeAtomic(childNode.child("value").attribute("value"), valueType);
			if (!valueVar.is_valid())
				CORE_LOG(E_Level::Error, E_Context::Core, "FAILED.");
		}
		else
		{
			auto valueNode = childNode.child("value");
			if (!valueNode)
			{
				CORE_LOG(E_Level::Error, E_Context::Core, "Missing value node in associative array.");
			}
			const auto node = *valueNode.children().begin();
			const auto type = rttr::type::get_by_name(node.name());
			if (type.is_valid() == false)
				continue;
			valueVar = type.create();
			if (!valueVar.is_valid())
			{
				CORE_LOG(E_Level::Error, E_Context::Core, "Could not instantiate type '{}'", type);
				continue;
			}
			valueVar = DeserializeNode(node, valueVar);
			if (!valueVar.convert(view.get_value_type()))
			{
				CORE_LOG(E_Level::Error, E_Context::Core, "Cannot convert to the value type {} to {}.", valueVar.get_type(), view.get_value_type());
				continue;
			}
			FinishDeserialization(type, valueVar);
		}

		if (view.insert(keyVar, valueVar).second == false)
		{
			CORE_LOG(E_Level::Error, E_Context::Core, "Something happened.");
		}
	}
}

//=================================================================================
void C_XMLDeserializer::FinishDeserialization(const rttr::type& type, const rttr::variant& var)
{
	if (const auto method = type.get_method("AfterDeserialize"))
	{
		if (method.get_parameter_infos().empty())
		{
			method.invoke(var);
		}
		else
		{
			GLE_ASSERT(method.get_parameter_infos().size() == 1, "Expect only one parameter for 'AfterDeserialize' method");
			GLE_ASSERT(method.get_parameter_infos().begin()->get_type() == rttr::type::get<DeserializeCtx>(),
					   "Expect the only one parameter for 'AfterDeserialize' method to be {}", rttr::type::get<DeserializeCtx>());
			method.invoke(var, m_Ctx);
		}
	}
}
namespace {
template <class T>
const auto setter = [](rttr::variant& instance, const auto& value) {
	if (instance.get_type().is_wrapper())
	{
		rttr::variant_cast<std::reference_wrapper<T>>(instance).get() = value;
	}
	else
	{
		instance = value;
	}
};
}
//=================================================================================
void C_XMLDeserializer::DeserializeAtomic(const pugi::xml_attribute& attr, const rttr::type& type, rttr::variant& instance)
{

	if (attr)
	{
		if (type == rttr::type::get<bool>())
		{
			setter<bool>(instance, attr.as_bool());
			return;
		}
		else if (type == rttr::type::get<char>())
		{
			setter<char>(instance, static_cast<char>(attr.as_string()[0]));
			return;
		}
		else if (type == rttr::type::get<short>())
		{
			setter<short>(instance, static_cast<short>(attr.as_int()));
			return;
		}
		else if (type == rttr::type::get<int8_t>())
		{
			setter<int8_t>(instance, attr.as_int());
			return;
		}
		else if (type == rttr::type::get<int16_t>())
		{
			setter<int16_t>(instance, attr.as_int());
			return;
		}
		else if (type == rttr::type::get<int32_t>())
		{
			setter<int32_t>(instance, attr.as_int());
			return;
		}
		else if (type == rttr::type::get<int64_t>())
		{
			setter<int64_t>(instance, attr.as_llong());
			return;
		}
		else if (type == rttr::type::get<long>())
		{
			setter<int64_t>(instance, static_cast<long>(attr.as_llong()));
			return;
		}
		else if (type == rttr::type::get<uint8_t>())
		{
			setter<uint8_t>(instance, attr.as_uint());
			return;
		}
		else if (type == rttr::type::get<uint16_t>())
		{
			setter<uint16_t>(instance, attr.as_uint());
			return;
		}
		else if (type == rttr::type::get<uint32_t>())
		{
			setter<uint32_t>(instance, attr.as_uint());
			return;
		}
		else if (type == rttr::type::get<uint64_t>())
		{
			setter<uint64_t>(instance, attr.as_ullong());
			return;
		}
		else if (type == rttr::type::get<float>())
		{
			setter<float>(instance, attr.as_float());
			return;
		}
		else if (type == rttr::type::get<double>())
		{
			setter<double>(instance, attr.as_double());
			return;
		}
		else if (type == rttr::type::get<std::string>())
		{
			setter<std::string>(instance, std::string(attr.as_string()));
			return;
		}
		else if (type == rttr::type::get<std::filesystem::path>())
		{
			setter<std::filesystem::path>(instance, std::filesystem::path(attr.as_string()));
			return;
		}
		else if (type.is_enumeration())
		{
			if (instance.get_type().is_wrapper())
			{
				rttr::variant_cast<std::reference_wrapper<int>>(instance).get() = type.get_enumeration().name_to_value(attr.as_string()).convert(type.get_enumeration().get_underlying_type());
			}
			else
			{
				instance = type.get_enumeration().name_to_value(attr.as_string());
			}
			return;
		}
	}
	CORE_LOG(E_Level::Error, E_Context::Core, "Unknown atomics type {} attribute {}", type, attr.name());
}

} // namespace GLEngine::Utils
