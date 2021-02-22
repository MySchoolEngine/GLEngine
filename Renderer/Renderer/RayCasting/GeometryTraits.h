#pragma once

namespace GLEngine::Physics::Primitives {
struct S_Plane;
struct S_Disc;
struct S_Sphere;
struct S_Triangle;
} // namespace GLEngine::Physics::Primitives

namespace GLEngine::Renderer {
class I_Sampler;
}

namespace GLEngine::Renderer::RayTracing {

class T_GeometryTraits {
public:
	static float GetArea(const Physics::Primitives::S_Plane&);
	static float GetArea(const Physics::Primitives::S_Disc& disc);
	static float GetArea(const Physics::Primitives::S_Sphere&);
	static float GetArea(const Physics::Primitives::S_Triangle& triangle);

	static glm::vec3 SamplePoint(const Physics::Primitives::S_Disc& disc, I_Sampler* rnd);
	static glm::vec3 SamplePoint(const Physics::Primitives::S_Triangle& triangle, I_Sampler* rnd);
};

} // namespace GLEngine::Renderer::RayTracing
