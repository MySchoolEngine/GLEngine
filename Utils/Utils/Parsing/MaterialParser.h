#pragma once

#include <Utils/UtilsApi.h>

#include <glm/glm.hpp>

#include <pugixml.hpp>

namespace Utils::Parsing {
struct MaterialData {
	glm::vec3	m_Color			= glm::vec3(1, 0, 0);
	float		m_Roughness		= 0;
	std::string m_ColorMap		= "";
	std::string m_RoughtnessMap = "";
	std::string m_NormalMap		= "";
	std::string m_MaterialName	= "basic";
};
class UTILS_API_EXPORT C_MaterialParser {
public:
	[[nodiscard]] static MaterialData ParseMaterialData(const pugi::xml_node& node);
};
} // namespace Utils::Parsing
