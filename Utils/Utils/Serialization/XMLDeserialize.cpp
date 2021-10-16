#include <Utils/Serialization/GLMRTTI.h>
#include <Utils/Serialization/XMLDeserialize.h>

#include <rttr/type>

namespace GLEngine::Utils {

//=================================================================================
rttr::variant C_XMLDeserializer::DeserializeDoc(const pugi::xml_document& document)
{
	return DeserializeNode(document.root().first_child());
}

//=================================================================================
rttr::variant C_XMLDeserializer::DeserializeNode(const pugi::xml_node& node)
{
	const auto type = rttr::type::get_by_name(node.name());
	if (type.is_valid() == false)
		return {};
	auto var = type.create();
	for (auto& prop : type.get_properties()) {
		
	}

	return var;
}

//=================================================================================
void C_XMLDeserializer::DeserializeProperty(rttr::property& prop, rttr::variant& owner, const pugi::xml_node& node)
{
	const auto type = prop.get_type();
	if (type.is_arithmetic())
	{
		prop.set_value(owner, DeserializeAtomic(node, prop));
	}
}

//=================================================================================
rttr::variant C_XMLDeserializer::DeserializeAtomic(const pugi::xml_node& node, rttr::property& prop)
{
	const auto attribute = node.attribute(prop.get_name().to_string().c_str());

}

}
