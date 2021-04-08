#include <Utils/Parsing/MatrixParse.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <pugixml.hpp>

#include <sstream>

namespace Utils::Parsing {

//=================================================================================
glm::mat4 C_MatrixParser::ParseTransformation(const pugi::xml_node& node)
{
	if (auto translation = node.child("translate"))
	{
		float			  x, y, z;
		std::stringstream ss;
		ss << translation.child_value();
		ss >> x >> y >> z;
		return glm::translate(glm::mat4(1.f), {x, y, z});
	}
	return glm::mat4(1.f);
}

//=================================================================================
glm::mat4 C_MatrixParser::ParseRotations(const pugi::xml_node& node)
{
	glm::mat4 rot(1.0f);
	for (auto rotate : node.children("rotate"))
	{
		float			  x, y, z, w;
		std::stringstream ss;
		ss << rotate.child_value();
		ss >> x >> y >> z >> w;

		rot = rot * glm::rotate(glm::radians(w), glm::vec3(x, y, z));
	}

	return rot;
}

//=================================================================================
glm::mat4 C_MatrixParser::ParseScale(const pugi::xml_node& node)
{
	if (auto translation = node.child("scale"))
	{
		float			  x, y, z;
		std::stringstream ss;
		ss << translation.child_value();
		ss >> x >> y >> z;
		return glm::scale(glm::mat4(1.f), {x, y, z});
	}
	return glm::mat4(1.f);
}

} // namespace Utils::Parsing