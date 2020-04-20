#pragma once

#include <Utils/UtilsApi.h>

#include <glm/glm.hpp>
#include <pugixml.hpp>

namespace Utils::Parsing
{
class UTILS_API_EXPORT C_ColorParser
{
public:
	static glm::vec3 ParseColorRGB(const pugi::xml_node& node);
};
}