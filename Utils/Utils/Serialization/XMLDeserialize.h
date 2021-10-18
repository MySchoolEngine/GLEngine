#pragma once

#include <Utils/UtilsApi.h>

#include <pugixml.hpp>

#include <optional>
#include <rttr/type>

namespace GLEngine::Utils {
class UTILS_API_EXPORT C_XMLDeserializer {
public:
	C_XMLDeserializer() = default;
	template <class T> std::optional<T> Deserialize(const pugi::xml_document& document)
	{
		auto var = DeserializeDoc(document);
		if (var.can_convert<T>())
		{
			return var.convert<T>();
		}
		return {};
	}

private:
	rttr::variant DeserializeDoc(const pugi::xml_document& document);
	rttr::variant DeserializeNode(const pugi::xml_node& node, const rttr::type& type);
	void		  DeserializeProperty(const rttr::property& prop, rttr::variant& owner, const pugi::xml_node& node);
	rttr::variant DeserializeAtomic(const pugi::xml_node& node, const rttr::property& prop);
};
} // namespace GLEngine::Utils
