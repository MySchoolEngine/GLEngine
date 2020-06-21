#pragma once

#include <Utils/UtilsApi.h>

#include <glm/glm.hpp>
#include <pugixml.hpp>

namespace Utils::Parsing
{
class UTILS_API_EXPORT C_MatrixParser
{
public:
	static glm::mat4 ParseTransformation(const pugi::xml_node& node);
	static glm::mat4 ParseRotations(const pugi::xml_node& node);
	static glm::mat4 ParseScale(const pugi::xml_node& node);
};
}