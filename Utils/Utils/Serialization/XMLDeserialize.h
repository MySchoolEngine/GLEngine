#pragma once

#include <Utils/UtilsApi.h>

#include <memory>
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
	C_XMLDeserializer(Core::C_ResourceManager& resMng, bool loadHandlesInstantly);
	template <class T> std::optional<T> Deserialize(const pugi::xml_document& document)
	{
		auto var = DeserializeDoc(document);
		if (var.can_convert<T>())
		{
			return var.convert<T>();
		}
		// type.create() commonly hands back a std::shared_ptr<T> (RTTR's default constructor policy
		// is as_std_shared_ptr), for which there is no registered "unwrap to T by value" converter -
		// nor should every deserializable type need to register one just to support this pattern. T
		// is known statically here, so pull the shared_ptr out ourselves and move the pointee into
		// the optional instead.
		if (var.can_convert<std::shared_ptr<T>>())
		{
			if (auto ptr = var.convert<std::shared_ptr<T>>())
				return std::optional<T>(std::move(*ptr));
		}
		return {};
	}

	struct DeserializeCtx {
		Core::C_ResourceManager& m_ResMng;
		bool					 bLoadHandlesInstantly;
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
	bool		   m_IsRootObject = true;
};
} // namespace GLEngine::Utils
