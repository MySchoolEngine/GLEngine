#pragma once

#include <Utils/UtilsApi.h>

#include <rttr/type>
#include <pugixml.hpp>

namespace GLEngine::Utils {
class UTILS_API_EXPORT C_XMLSerializer {
public:
	[[nodiscard]] std::string Serialize(const rttr::instance obj);

private:
	[[nodiscard]] pugi::xml_node SerializeObject(const rttr::instance& obj2, pugi::xml_node& node);
	void						 WriteProperty(const rttr::property& prop, const rttr::instance& var, pugi::xml_node& parent);
	void						 WriteAtomics(const rttr::property& prop, const rttr::variant& obj, pugi::xml_node& attr);
	void						 WriteArray(const rttr::variant_sequential_view& view, pugi::xml_node& parent);

	rttr::string_view GetNodeName(const rttr::type& type);
};
}