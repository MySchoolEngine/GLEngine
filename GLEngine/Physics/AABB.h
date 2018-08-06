#pragma once

#include <glm/glm.hpp>

#include <Physics/Shapes.h>

namespace phys {
struct S_AABB {
public:
	glm::vec3 m_Min;
	glm::vec3 m_Max;
	S_AABB();

	void Add(const glm::vec3& point);
	void Add(const glm::vec4& point);
	void Add(const S_AABB& bbox);
	void Add(const Shapes::S_Sphere& sphere);

	Shapes::S_Sphere GetSphere() const;
};
}