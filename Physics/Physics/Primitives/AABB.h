#pragma once

#include <Physics/PhysicsApi.h>

#include <glm/glm.hpp>


namespace GLEngine {
namespace Physics {
namespace Primitives {

struct S_Sphere;

struct PHYSICS_API_EXPORT S_AABB {
public:
#pragma warning(push)
#pragma warning( disable : 4251)
	glm::vec3 m_Min;
	glm::vec3 m_Max;
#pragma warning( pop ) 
	S_AABB();

	void Add(const glm::vec3& point);
	void Add(const glm::vec4& point);
	void Add(const S_AABB& bbox);
	void Add(const S_Sphere& sphere);


	void updateWithTriangle(const glm::vec4* triangleVertices);

	S_Sphere GetSphere() const;
	S_AABB getTransformedAABB(const glm::mat4 matrix) const;
};
}
}
}