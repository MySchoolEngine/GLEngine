#pragma once

#include <glm/glm.hpp>

#include <iostream>

namespace glm {

inline std::ostream& operator<<(std::ostream& stream, const vec2& vec)
{
	return stream << "glm::vec3(" << vec.x << ", " << vec.y << ")";
}

inline std::ostream& operator<<(std::ostream& stream, const vec3& vec)
{
	return stream << "glm::vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
}

inline std::ostream& operator<<(std::ostream& stream, const vec4& vec)
{
	return stream << "glm::vec4(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
}
}

