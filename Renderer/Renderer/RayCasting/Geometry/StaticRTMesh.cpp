#include <RendererStdafx.h>

#include <Renderer/Materials/PBRMaterialData.h>
#include <Renderer/RayCasting/Geometry/StaticRTMesh.h>

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
	const auto							  rayTransformed = ray.GetTransformedRay(m_InvTransform);
	const auto&							  trimeshes		 = m_TrimeshModel.GetResource().GetTrimeshes();
	C_RayIntersection					  bestIntersection;
	float								  tMaxCurrent  = tMax;
	I_MaterialInterface*				  bestMaterial = nullptr;
	Core::ResourceHandle<TextureResource> bestAlphaMap;
	for (const auto [trimesh, material, alphaMap] : std::views::zip(trimeshes, m_Materials, m_AlphaMaps))
	{
		C_RayIntersection intersectionCandidate;
		if (trimesh.Intersect(rayTransformed, intersectionCandidate, tMaxCurrent))
		{
			// because we limit via bestIntersection.GetRayLength() this have to be better hit
			bestIntersection = intersectionCandidate;
			tMaxCurrent		 = std::min(tMax, bestIntersection.GetRayLength());
			bestMaterial	 = material;
			bestAlphaMap	 = alphaMap;
		}
	}
	if (std::isinf(bestIntersection.GetRayLength()))
		return false;
	intersection = bestIntersection;
	// material
	intersection.SetMaterial(bestMaterial);
	// alpha mask
	if (bestAlphaMap)
	{
		intersection.SetAlphaMask(C_TextureView(const_cast<I_TextureViewStorage*>(&bestAlphaMap.GetResource().GetStorage())));
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

//=================================================================================
void C_StaticRTMesh::InitMaterials(I_MaterialProviderInterface& materialProvider)
{
	if (m_TrimeshModel.IsReady() == false)
		return;

	m_Materials.reserve(m_TrimeshModel.GetResource().GetTrimeshes().size());
	m_AlphaMaps.reserve(m_TrimeshModel.GetResource().GetTrimeshes().size());
	for (const auto& trimesh : m_TrimeshModel.GetResource().GetTrimeshes())
	{
		m_Materials.emplace_back(materialProvider.AddMaterial(trimesh.GetMaterialHandle()));
		if (auto* pbrData = dynamic_cast<const C_PBRMaterialData*>(trimesh.GetMaterialHandle().GetResource().GetMaterialData()))
		{
			if (pbrData->GetUseTransparency())
				m_AlphaMaps.emplace_back(pbrData->GetColorMapRes());
			else
				m_AlphaMaps.emplace_back();
		}
		else
			m_AlphaMaps.emplace_back();
	}
}
} // namespace GLEngine::Renderer