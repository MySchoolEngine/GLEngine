#pragma once

#include <Renderer/RayCasting/Geometry/PrimitiveObject.h>
#include <Renderer/RayCasting/Light/ILight.h>

namespace GLEngine::Physics::Primitives {
struct S_Disc;
} // namespace GLEngine::Physics::Primitives

namespace GLEngine::Renderer {
class I_RayGeometryObject;
class I_Sampler;
class C_RayIntersection;
} // namespace GLEngine::Renderer

namespace GLEngine::Renderer::RayTracing {
class C_AreaLight : public I_RayLight {
public:
	template <class primitiveT>
	C_AreaLight(const Colours::T_Colour& radiance, const std::shared_ptr<C_Primitive<primitiveT>>& shape)
		: m_Radiance(radiance)
		, m_Shape(std::make_unique<Geometry<primitiveT>>(shape))
	{
	}
	[[nodiscard]] Colours::T_Colour SampleLi(const C_RayIntersection& intersection, I_Sampler& rnd, S_VisibilityTester& vis, float* pdf) const override;
	[[nodiscard]] Colours::T_Colour Le() const override;
	bool							IsDeltaLight() const override { return false; }
	float							Pdf_Li(const glm::vec3& wi) const override;

	[[nodiscard]] std::shared_ptr<I_RayGeometryObject> GetGeometry() const;

private:
	// type erasure due to bad architecture
	struct GeometryBase {
		virtual ~GeometryBase()														   = default;
		virtual float								 Area() const					   = 0;
		virtual glm::vec3							 Normal() const					   = 0;
		virtual glm::vec3							 SamplePoint(I_Sampler& rnd) const = 0;
		virtual std::shared_ptr<I_RayGeometryObject> GetGeometry() const			   = 0;
	};

	template <class primitiveT>
	struct Geometry : public GeometryBase {
		explicit Geometry(std::shared_ptr<C_Primitive<primitiveT>> pShape)
			: shape(pShape)
		{
		}
		~Geometry() override = default;
		float								 Area() const override { return shape->Area(); }
		glm::vec3							 Normal() const override { return RayTracing::T_GeometryTraits::GetNormal(shape->m_Primitive); }
		glm::vec3							 SamplePoint(I_Sampler& rnd) const override { return RayTracing::T_GeometryTraits::SamplePoint(shape->m_Primitive, rnd); }
		std::shared_ptr<I_RayGeometryObject> GetGeometry() const override { return shape; }

		std::shared_ptr<C_Primitive<primitiveT>> shape;
	};

	Colours::T_Colour			  m_Radiance; // TODO: Terrible choice. Radiance is bad choice
	std::unique_ptr<GeometryBase> m_Shape;
};
} // namespace GLEngine::Renderer::RayTracing
