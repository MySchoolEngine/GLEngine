#include <RendererStdafx.h>

#include <Renderer/RayCasting/Geometry/StaticRTMesh.h>


//		.property("Transform", &C_Trimesh::m_Transform)(rttr::policy::prop::as_reference_wrapper, REGISTER_DEFAULT_VALUE(glm::mat4(1.f)))
namespace GLEngine::Renderer {

//=================================================================================
C_StaticRTMesh::C_StaticRTMesh(const Core::ResourceHandle<C_TrimeshModel>& model)
	: m_TrimeshModel(model)
	, m_AABB(model.GetResource().GetAABB())
{
	GLE_ASSERT(model.IsReady(), "Only loaded C_TrimeshModel can be used to construct C_StaticRTMesh");
}

//=================================================================================
C_StaticRTMesh::~C_StaticRTMesh() = default;

//=================================================================================
bool C_StaticRTMesh::Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, const float tMax) const
{
	const auto	rayTransformed = ray.GetTransformedRay(m_InvTransform);
	const auto& trimeshes	   = m_TrimeshModel.GetResource().GetTrimeshes();
	C_RayIntersection bestIntersection;
	float			  tMaxCurrent = tMax;
	for (const auto& trimesh : trimeshes)
	{
		C_RayIntersection intersectionCandidate;
		if (trimesh.Intersect(rayTransformed, intersectionCandidate, tMaxCurrent))
		{
			// because we limit via bestIntersection.GetRayLength() this have to be better hit
			bestIntersection = intersectionCandidate;
			tMaxCurrent		 = std::min(tMax, bestIntersection.GetRayLength());
		}
	}
	if (std::isinf(bestIntersection.GetRayLength()))
		return false;
	intersection = bestIntersection;
	// material
	intersection.SetMaterial(&GetMaterial());
	// alpha mask
	if (m_AlphaMask.IsReady())
	{
		intersection.SetAlphaMask(C_TextureView(const_cast<I_TextureViewStorage*>(&m_AlphaMask.GetResource().GetStorage())));
	}
	// normal correction
	auto normal = intersection.GetFrame().Normal();
	normal		= glm::transpose(m_InvTransform) * glm::vec4(normal, 0.f);
	intersection.SetFrame(S_Frame(normal));
	intersection.TransformRayAndPoint(m_Transform);

	return true;
}

//=================================================================================
float C_StaticRTMesh::Area() const
{
	return 0.f;
}

//=================================================================================
void C_StaticRTMesh::SetTransformation(const glm::mat4& mat)
{
	auto invMat	   = glm::inverse(mat);
	m_AABB		   = m_AABB.getTransformedAABB(mat);
	m_Transform	   = mat;
	m_InvTransform = invMat;
}
} // namespace GLEngine::Renderer