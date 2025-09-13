#pragma once

#include <Utils/UtilsApi.h>

#include <optional>
#include <rttr/type>

/**
 * If you want to do any logic after deserialization, please define method with following header
 * and register it in RTTR
 * void AfterDeserialize();
 * or
 * void AfterDeserialize(GLEngine::Utils::C_XMLDeserializer::DeserializeCtx& ctx);
 */

namespace GLEngine::Core {
class C_ResourceManager;
}

namespace pugi {
class xml_document;
class xml_node;
class xml_attribute;
} // namespace pugi

namespace GLEngine::Utils {
class UTILS_API_EXPORT C_XMLDeserializer {
public:
	C_XMLDeserializer(Core::C_ResourceManager& resMng);
	template <class T> std::optional<T> Deserialize(const pugi::xml_document& document)
	{
		auto var = DeserializeDoc(document);
		if (var.can_convert<T>())
		{
			return var.convert<T>();
		}
		return {};
	}

	struct DeserializeCtx {
		Core::C_ResourceManager& m_ResMng;
	};

private:
	rttr::variant DeserializeDoc(const pugi::xml_document& document);
	rttr::variant DeserializeNode(const pugi::xml_node& node, rttr::variant& var);
	void		  DeserializeProperty(const rttr::property& prop, rttr::variant& owner, const pugi::xml_node& node);
	void		  DeserializeAtomic(const pugi::xml_attribute& attr, const rttr::type& type, rttr::variant& instance);
	void		  DeserializeArray(const pugi::xml_node& child, rttr::variant_sequential_view& view);
	void		  DeserializeAssociativeArray(const pugi::xml_node& child, rttr::variant_associative_view& view);
	void		  FinishDeserialization(const rttr::type& type, const rttr::variant& var);

	DeserializeCtx m_Ctx;
};
} // namespace GLEngine::Utils
