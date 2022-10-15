#include <Utils/Serialization/GLMRTTI.h>

#include <glm/glm.hpp>
#include <rttr/registration>

RTTR_REGISTRATION
{
	rttr::registration::class_<glm::vec2>("vec2")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("x", &glm::vec2::x)
		.property("y", &glm::vec2::y);
	rttr::registration::class_<glm::vec3>("vec3")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("x", &glm::vec3::x)
		.property("y", &glm::vec3::y)
		.property("z", &glm::vec3::z);
	rttr::registration::class_<glm::vec4>("vec4")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("x", &glm::vec4::x)
		.property("y", &glm::vec4::y)
		.property("z", &glm::vec4::z)
		.property("w", &glm::vec4::w);
}