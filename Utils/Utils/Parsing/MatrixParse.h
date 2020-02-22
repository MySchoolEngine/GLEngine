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
};
}