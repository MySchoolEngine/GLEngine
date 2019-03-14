#pragma once

#include <Core/CoreMacros.h>

#include <Physics/Primitives/Shapes.h>

#include <glm/glm.hpp>

namespace Physics {
namespace Primitives {
struct API_EXPORT S_AABB {
public:
	glm::vec3 m_Min;
	glm::vec3 m_Max;
	S_AABB();

	void Add(const glm::vec3& point);
	void Add(const glm::vec4& point);
	void Add(const S_AABB& bbox);
	void Add(const S_Sphere& sphere);

	S_Sphere GetSphere() const;
};
}
}