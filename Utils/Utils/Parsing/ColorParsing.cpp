#include <Utils/Parsing/ColorParsing.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <pugixml.hpp>

#include <sstream>

namespace Utils::Parsing {

//=================================================================================
glm::vec3 C_ColorParser::ParseColorRGB(const pugi::xml_node& node, std::string_view nodeName)
{
	glm::vec3 ret(0.f);
	if (auto colorChild = node.child(nodeName.data()))
	{
		std::stringstream ss;
		ss << colorChild.child_value();
		ss >> ret.r >> ret.g >> ret.b;
	}

	return ret;
}

} // namespace Utils::Parsing