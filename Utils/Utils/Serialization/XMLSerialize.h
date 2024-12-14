#pragma once

#include <Utils/UtilsApi.h>

#include <pugixml.hpp>

#include <rttr/type>

namespace GLEngine::Utils {
class UTILS_API_EXPORT C_XMLSerializer {
public:
	[[nodiscard]] pugi::xml_document Serialize(const rttr::instance obj);

private:
	pugi::xml_node SerializeObject(const rttr::instance& obj2, pugi::xml_node node);
	void		   WriteProperty(const rttr::property& prop, const rttr::instance& var, pugi::xml_node parent);
	static void	   WriteAtomics(const rttr::type& type, const rttr::variant& obj, pugi::xml_attribute attr);
	void		   WriteArray(const rttr::variant_sequential_view& view, pugi::xml_node parent);
	void		   WriteAssociativeArray(const rttr::variant_associative_view& view, pugi::xml_node parent);

	static rttr::string_view GetNodeName(const rttr::type& type);
};
} // namespace GLEngine::Utils