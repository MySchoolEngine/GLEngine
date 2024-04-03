#pragma once

namespace GLEngine::Physics::Primitives {
struct S_Plane;
struct S_Disc;
struct S_Sphere;
struct S_Triangle;
struct S_Ray;
} // namespace GLEngine::Physics::Primitives

namespace GLEngine::Renderer {
class I_Sampler;
class C_RayIntersection;
} // namespace GLEngine::Renderer

namespace GLEngine::Renderer::RayTracing {

class T_GeometryTraits {
public:
	static float GetArea(const Physics::Primitives::S_Plane&);
	static float GetArea(const Physics::Primitives::S_Disc& disc);
	static float GetArea(const Physics::Primitives::S_Sphere&);
	static float GetArea(const Physics::Primitives::S_Triangle& triangle);

	static glm::vec3 GetNormal(const Physics::Primitives::S_Triangle& triangle);
	static glm::vec3 GetNormal(const Physics::Primitives::S_Plane&);
	static glm::vec3 GetNormal(const Physics::Primitives::S_Disc& disc);

	static glm::vec3 SamplePoint(const Physics::Primitives::S_Disc& disc, I_Sampler& rnd);
	static glm::vec3 SamplePoint(const Physics::Primitives::S_Triangle& triangle, I_Sampler& rnd);

	static void FillIntersection(const Physics::Primitives::S_Plane& plane, float t, const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection);
	static void FillIntersection(const Physics::Primitives::S_Disc& disc, float t, const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection);
	static void FillIntersection(const Physics::Primitives::S_Sphere& sphere, float t, const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection);
	static void FillIntersection(const Physics::Primitives::S_Triangle& triangle, float t, const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection);
};

} // namespace GLEngine::Renderer::RayTracing
