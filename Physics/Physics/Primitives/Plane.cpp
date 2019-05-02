#include <Physics/Primitives/Plane.h>

#include <Physics/Primitives/Ray.h>


namespace GLEngine {
namespace Physics {
namespace Primitives {


//=================================================================================
float S_Plane::Intersect(const S_Ray& ray) const
{
	return -(glm::dot(ray.origin, noraml) - originOffset) / (glm::dot(ray.direction, noraml));
}

}}}