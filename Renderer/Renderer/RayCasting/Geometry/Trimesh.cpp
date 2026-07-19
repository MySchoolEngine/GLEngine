#include <RendererStdafx.h>

#include <Renderer/DebugDraw.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/RayCasting/Geometry/GeometryTraits.h>
#include <Renderer/RayCasting/Geometry/Trimesh.h>
#include <Renderer/RayCasting/RayIntersection.h>

#include <Physics/GeometryUtils/TriangleIntersect.h>

#include <Utils/Range.h>
#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/SerializationUtils.h>

#include <algorithm>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_Trimesh>("Trimesh")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("Vertices", &C_Trimesh::m_Vertices)(rttr::policy::prop::as_reference_wrapper)
		.property("TexCoords", &C_Trimesh::m_TexCoords)(rttr::policy::prop::as_reference_wrapper)
		.property("Material", &C_Trimesh::m_Material)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetamember<SerializationCls::MandatoryProperty>(true),
			RegisterMetaclass<MetaGUI::MaterialResource>(),
			RegisterMetamember<UI::MaterialResource::Name>("Model"),
			REGISTER_DEFAULT_VALUE(GLEngine::Core::ResourceHandle<MaterialResource>()))
		.property("Transform", &C_Trimesh::m_Transform)(rttr::policy::prop::as_reference_wrapper, REGISTER_DEFAULT_VALUE(glm::mat4(1.f)))
		.method("AfterDeserialize", &C_Trimesh::AfterDeserialize)()
		.property("AABB", &C_Trimesh::m_AABB)
		.property("BVH", &C_Trimesh::m_BVH);
}
// clang-format on

namespace GLEngine::Renderer {
//=================================================================================
C_Trimesh::C_Trimesh() = default;

//=================================================================================
C_Trimesh::C_Trimesh(const C_Trimesh& other)
	: m_Vertices(other.m_Vertices)
	, m_TexCoords(other.m_TexCoords)
	, m_AABB(other.m_AABB)
	, m_Transform(other.m_Transform)
	, m_TransformInv(other.m_TransformInv)
	, m_BVH(nullptr)
	, m_Material(other.m_Material)
{
	if (other.m_BVH)
	{
		m_BVH				 = new BVH();
		m_BVH->m_Storage	 = &m_Vertices;
		m_BVH->m_Nodes		 = other.m_BVH->m_Nodes;
		m_BVH->m_LookupTable = other.m_BVH->m_LookupTable;
	}
}

//=================================================================================
C_Trimesh::C_Trimesh(C_Trimesh&& other) noexcept
	: m_Vertices(std::move(other.m_Vertices))
	, m_TexCoords(std::move(other.m_TexCoords))
	, m_AABB(std::move(other.m_AABB))
	, m_Transform(other.m_Transform)
	, m_TransformInv(other.m_TransformInv)
	, m_BVH(std::exchange(other.m_BVH, nullptr))
	, m_Material(other.m_Material)
{
	if (m_BVH)
		m_BVH->m_Storage = &m_Vertices;
}

//=================================================================================
C_Trimesh& C_Trimesh::operator=(const C_Trimesh& other)
{
	if (this != &other)
	{
		delete m_BVH;
		m_Vertices	   = other.m_Vertices;
		m_TexCoords	   = other.m_TexCoords;
		m_AABB		   = other.m_AABB;
		m_Transform	   = other.m_Transform;
		m_TransformInv = other.m_TransformInv;
		m_Material	   = other.m_Material;
		m_BVH		   = nullptr;
		if (other.m_BVH)
		{
			m_BVH				 = new BVH();
			m_BVH->m_Storage	 = &m_Vertices;
			m_BVH->m_Nodes		 = other.m_BVH->m_Nodes;
			m_BVH->m_LookupTable = other.m_BVH->m_LookupTable;
		}
	}
	return *this;
}

//=================================================================================
C_Trimesh& C_Trimesh::operator=(C_Trimesh&& other) noexcept
{
	if (this != &other)
	{
		delete m_BVH;
		m_Vertices	   = std::move(other.m_Vertices);
		m_TexCoords	   = std::move(other.m_TexCoords);
		m_AABB		   = std::move(other.m_AABB);
		m_Transform	   = other.m_Transform;
		m_TransformInv = other.m_TransformInv;
		m_BVH		   = std::exchange(other.m_BVH, nullptr);
		m_Material	   = other.m_Material;
		if (m_BVH)
			m_BVH->m_Storage = &m_Vertices;
	}
	return *this;
}

//=================================================================================
C_Trimesh::~C_Trimesh()
{
	delete m_BVH;
}

//=================================================================================
bool C_Trimesh::Intersect(const Physics::Primitives::S_Ray& rayIn, C_RayIntersection& intersection, const float tMax) const
{
	if (m_BVH)
	{
		const auto	 ray = rayIn.GetTransformedRay(m_TransformInv);
		glm::vec2	 barycentric;
		unsigned int triangleIndex;
		if (m_BVH->Intersect(ray, intersection, &triangleIndex, &barycentric))
		{
			// TODO transform normal
			// intersection.SetMaterial(&GetMaterial());
			intersection.TransformRayAndPoint(m_Transform);
			// intersection.SetRayLength(glm::distance(intersection.GetRay().origin, intersection.GetIntersectionPoint()));
			// if (m_AlphaMask.IsReady())
			// {
			// 	intersection.SetAlphaMask(C_TextureView(const_cast<I_TextureViewStorage*>(&m_AlphaMask.GetResource().GetStorage())));
			// }
			// UVs if present
			if (!m_TexCoords.empty())
			{
				glm::vec2		 uv;
				const glm::vec2* triUV = &(m_TexCoords[triangleIndex * 3]);
				RayTracing::T_GeometryTraits::BarycentricInterpolation(barycentric, triUV, uv);
				intersection.SetUV(uv);
			}
			return true;
		}
		return false;
	}

	return IntersectBruteforce(rayIn, intersection, tMax);
}

//=================================================================================
bool C_Trimesh::IntersectBruteforce(const Physics::Primitives::S_Ray& rayIn, C_RayIntersection& intersection, const float tMax) const
{
	// this check happens internally in BVH
	if (const auto tAABB = m_AABB.Intersects(rayIn); tAABB > tMax || std::isinf(tAABB))
		return false;

	const auto ray = rayIn.GetTransformedRay(m_TransformInv);

	struct S_IntersectionInfo {
		float		 t			= std::numeric_limits<float>::infinity();
		unsigned int vertexBase = 0;
		glm::vec2	 barycentric;
	} closestIntersect;

	glm::vec2  barycentric;
	const auto vertexCount = m_Vertices.size();

	for (unsigned int i = 0; i < vertexCount; i += 3)
	{
		const glm::vec3* triDef = &(m_Vertices[i]);
		const auto		 length = Physics::TriangleRayIntersect(triDef, ray, &barycentric);
		if (length < closestIntersect.t)
		{
			// inter.SetMaterial(&GetMaterial());

			closestIntersect = {.t = length, .vertexBase = i, .barycentric = barycentric};
		}
	}

	if (std::isinf(closestIntersect.t))
		return false;

	const unsigned int i = closestIntersect.vertexBase;
	// calculate normal
	auto normal = glm::cross(m_Vertices[i + 1] - m_Vertices[i], m_Vertices[i + 2] - m_Vertices[i]);
	normal		= glm::normalize(normal);
	normal		= glm::transpose(m_TransformInv) * glm::vec4(normal, 0.f);

	// initialize intersection
	intersection = C_RayIntersection(S_Frame(normal), ray.origin + closestIntersect.t * ray.direction, Physics::Primitives::S_Ray(ray));
	intersection.SetRayLength(closestIntersect.t);
	intersection.TransformRayAndPoint(m_Transform);

	// UVs if present
	if (!m_TexCoords.empty())
	{
		glm::vec2		 uv;
		const glm::vec2* triUV = &(m_TexCoords[i]);
		RayTracing::T_GeometryTraits::BarycentricInterpolation(barycentric, triUV, uv);
		intersection.SetUV(uv);
	}

	// if (m_AlphaMask.IsReady())
	//{
	//	intersection.SetAlphaMask(C_TextureView(const_cast<I_TextureViewStorage*>(&m_AlphaMask.GetResource().GetStorage())));
	// }
	return true;
}

//=================================================================================
void C_Trimesh::AddTriangle(const Physics::Primitives::S_Triangle& triangle)
{
	GLE_ASSERT(m_Vertices.size() % 3 == 0, "Wrong number of vertices.");
	m_Vertices.push_back(triangle.m_p[0]);
	m_Vertices.push_back(triangle.m_p[1]);
	m_Vertices.push_back(triangle.m_p[2]);
	m_AABB.updateWithTriangle(&(triangle.m_p[0]));
}

//=================================================================================
void C_Trimesh::AddTriangle(const Physics::Primitives::S_Triangle& triangle, const std::array<glm::vec2, 3>& uv)
{
	GLE_ASSERT(m_Vertices.size() == m_TexCoords.size(), "Mix matching uv and uv-less triangles.");
	AddTriangle(triangle);
	m_TexCoords.push_back(uv[0]);
	m_TexCoords.push_back(uv[1]);
	m_TexCoords.push_back(uv[2]);
}

//=================================================================================
void C_Trimesh::AddMesh(const MeshData::Mesh& mesh)
{
	m_Vertices	= mesh.vertices;
	m_TexCoords = mesh.texcoords;
	m_AABB		= mesh.bbox;
	GLE_ASSERT(m_Vertices.size() % 3 == 0, "Wrong number of vertices.");
}

//=================================================================================
void C_Trimesh::SetBVH(BVH* bvh)
{
	delete m_BVH;
	m_BVH = bvh;
}

//=================================================================================
void C_Trimesh::SetMaterialHandle(const Core::ResourceHandle<MaterialResource>& material)
{
	m_Material = material;
}

//=================================================================================
void C_Trimesh::SetTransformation(const glm::mat4& mat)
{
	auto invMat	   = glm::inverse(mat);
	m_AABB		   = m_AABB.getTransformedAABB(mat);
	m_Transform	   = mat;
	m_TransformInv = invMat;
}

//=================================================================================
std::size_t C_Trimesh::GetNumTriangles() const
{
	return m_Vertices.size() / 3;
}

//=================================================================================
void C_Trimesh::DebugDraw(I_DebugDraw& dd) const
{
	if (m_BVH)
		m_BVH->DebugDraw(dd, m_Transform);
}

//=================================================================================
void C_Trimesh::AfterDeserialize()
{
	m_TransformInv = glm::inverse(m_Transform);
	if (m_BVH)
	{
		m_BVH->m_Storage = &m_Vertices;
		CORE_LOG(E_Level::Info, E_Context::Render, "Loaded the BVH with depth of {}", m_BVH->ComputeMaxDepth());
	}
}

} // namespace GLEngine::Renderer
