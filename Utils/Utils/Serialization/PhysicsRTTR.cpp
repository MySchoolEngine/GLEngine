#include <CoreStdafx.h>

#include <Physics/Primitives/AABB.h>
#include <Physics/Primitives/Sphere.h>
#include <Physics/Primitives/Triangle.h>

#include <rttr/registration>

RTTR_REGISTRATION
{
	using namespace GLEngine::Physics::Primitives;

	rttr::registration::class_<S_AABB>("AABB")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("init", &S_AABB::m_Initialised)
		.property("min", &S_AABB::m_Min)
		.property("max", &S_AABB::m_Max);

	rttr::registration::class_<S_Sphere>("Sphere")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("position", &S_Sphere::m_position)
		.property("radius", &S_Sphere::m_radius);

	rttr::registration::class_<S_Triangle>("Triangle")
		.property("Vertices", &S_Triangle::m_p)
		.property("Normal", &S_Triangle::m_Normal)
		.property("Area", &S_Triangle::m_Area);
}
