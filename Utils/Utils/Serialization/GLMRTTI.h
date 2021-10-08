#pragma once


#include <glm/glm.hpp>
#include <rttr/registration>

RTTR_REGISTRATION
{
	rttr::registration::class_<glm::vec2>("vec2")
		.property("x", &glm::vec2::x)
		.property("y", &glm::vec2::y)
	;
	rttr::registration::class_<glm::vec3>("vec3")
		.property("x", &glm::vec3::x)
		.property("y", &glm::vec3::y)
		.property("z", &glm::vec3::z)
	;
	rttr::registration::class_<glm::vec4>("vec4")
		.property("x", &glm::vec4::x)
		.property("y", &glm::vec4::y)
		.property("z", &glm::vec4::z)
		.property("w", &glm::vec4::w)
	;
}

