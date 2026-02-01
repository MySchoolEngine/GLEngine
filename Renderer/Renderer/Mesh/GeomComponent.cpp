#include <RendererStdafx.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Materials/Material.h>
#include <Renderer/Materials/MaterialManager.h>
#include <Renderer/Mesh/GeomComponent.h>
#include <Renderer/Mesh/Geometry.h>
#include <Renderer/Renderer3D.h>
#include <Renderer/Resources/ResourceManager.h>

#include <GUI/ReflectionGUI.h>

#include <Core/Application.h>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_GeomComponent>("C_GeomComponent")
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("Material", &C_GeomComponent::m_Material)
		.method("AfterDeserialize", &C_GeomComponent::AfterDeserialize)()
		;

	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_GeomComponent>>();
	rttr::type::register_converter_func([](std::shared_ptr<C_GeomComponent> ptr, bool& ok) -> std::shared_ptr<GLEngine::Entity::I_Component> {
		ok = true;
		return std::static_pointer_cast<GLEngine::Entity::I_Component>(ptr);
	});
}
// clang-format on

namespace GLEngine::Renderer {

//=================================================================================
C_GeomComponent::C_GeomComponent()
	: I_RenderableComponent(nullptr)
{
	SetupGeometry(MeshData::C_Geometry::CreatePlane());
	auto& materialManager = C_MaterialManager::Instance();
	m_Material			  = materialManager.GetMaterial("GeomComponent");
	if (!m_Material)
	{
		m_Material = materialManager.RegisterMaterial(C_Material("GeomComponent"));
	}
}

//=================================================================================
C_GeomComponent::C_GeomComponent(const std::shared_ptr<Entity::I_Entity>& owner)
	: I_RenderableComponent(owner)
{
	SetupGeometry(MeshData::C_Geometry::CreatePlane());
	auto& materialManager = C_MaterialManager::Instance();
	m_Material			  = materialManager.GetMaterial("GeomComponent");
	if (!m_Material)
	{
		m_Material = materialManager.RegisterMaterial(C_Material("GeomComponent"));
	}
}

//=================================================================================
C_GeomComponent::~C_GeomComponent()
{
	I_Renderer&		 renderer = Core::C_Application::Get().GetActiveRenderer();
	ResourceManager& rm		  = renderer.GetRM();
	rm.destroyBuffer(m_Mesh.m_PositionsHandle);
	rm.destroyBuffer(m_Mesh.m_NormalsHandle);
	rm.destroyBuffer(m_Mesh.m_TexCoordsHandle);
	rm.destroyBuffer(m_Mesh.m_TangentHandle);
	rm.destroyBuffer(m_Mesh.m_BitangentHandle);
}

//=================================================================================
void C_GeomComponent::Update()
{
	if (m_Pipeline.IsValid() == false)
	{
		GLE_TODO("6-11-2024", "RohacekD", "Can we validate the definition with shaders?");
		m_Pipeline = Core::C_Application::Get().GetActiveRenderer().GetRM().createPipeline(PipelineDescriptor{
		.primitiveType = E_RenderPrimitives::TriangleList,
		.bindingCount = 5,
		.vertexInput   = {{
							  .binding = 0,
							  .type	   = Renderer::T_TypeShaderDataType_v<glm::vec3>
						  },
						  {
							  .binding = 1,
							  .type	   = Renderer::T_TypeShaderDataType_v<glm::vec3>
						  },
						  {
							  .binding = 2,
							  .type	   = Renderer::T_TypeShaderDataType_v<glm::vec2>
						  },
						  {
							  .binding = 3,
							  .type	   = Renderer::T_TypeShaderDataType_v<glm::vec3>
						  },
						  {
							  .binding = 4,
							  .type	   = Renderer::T_TypeShaderDataType_v<glm::vec3>
						  },
			},
		.shader =  "basicTracing",
		// .colorAttachmentFormat = GetTextureFormat(m_SwapChainImageFormat), no need for OpenGL and will be solved for Vulkan
	});
	}
	I_RenderableComponent::Update();
}

//=================================================================================
void C_GeomComponent::SetupGeometry(const MeshData::Mesh& mesh)
{
	I_Renderer&		 renderer = Core::C_Application::Get().GetActiveRenderer();
	ResourceManager& rm		  = renderer.GetRM();

	m_Mesh.m_NumPrimitives	 = mesh.vertices.size();
	const auto positionsSize = static_cast<uint32_t>(sizeof(mesh.vertices[0]) * mesh.vertices.size());
	m_Mesh.m_PositionsHandle = rm.createBuffer(BufferDescriptor{
		.size  = positionsSize,
		.type  = E_BufferType::Vertex,
		.usage = E_ResourceUsage::Immutable,
	});
	renderer.SetBufferData(m_Mesh.m_PositionsHandle, positionsSize, mesh.vertices.data());

	const auto normalsSize = static_cast<uint32_t>(sizeof(mesh.normals[0]) * mesh.normals.size());
	m_Mesh.m_NormalsHandle = rm.createBuffer(BufferDescriptor{
		.size  = normalsSize,
		.type  = E_BufferType::Vertex,
		.usage = E_ResourceUsage::Immutable,
	});
	renderer.SetBufferData(m_Mesh.m_NormalsHandle, normalsSize, mesh.normals.data());

	const auto texCoordSize	 = static_cast<uint32_t>(sizeof(mesh.texcoords[0]) * mesh.texcoords.size());
	m_Mesh.m_TexCoordsHandle = rm.createBuffer(BufferDescriptor{
		.size  = texCoordSize,
		.type  = E_BufferType::Vertex,
		.usage = E_ResourceUsage::Immutable,
	});
	renderer.SetBufferData(m_Mesh.m_TexCoordsHandle, texCoordSize, mesh.texcoords.data());

	const auto tangentSize = static_cast<uint32_t>(sizeof(mesh.tangent[0]) * mesh.tangent.size());
	m_Mesh.m_TangentHandle = rm.createBuffer(BufferDescriptor{
		.size  = tangentSize,
		.type  = E_BufferType::Vertex,
		.usage = E_ResourceUsage::Immutable,
	});
	renderer.SetBufferData(m_Mesh.m_TangentHandle, tangentSize, mesh.tangent.data());

	const auto bitangentSize = static_cast<uint32_t>(sizeof(mesh.bitangent[0]) * mesh.bitangent.size());
	m_Mesh.m_BitangentHandle = rm.createBuffer(BufferDescriptor{
		.size  = bitangentSize,
		.type  = E_BufferType::Vertex,
		.usage = E_ResourceUsage::Immutable,
	});
	renderer.SetBufferData(m_Mesh.m_BitangentHandle, bitangentSize, mesh.bitangent.data());
}

//=================================================================================
std::string_view C_GeomComponent::GetDebugComponentName() const
{
	return "GeomComponent";
}

//=================================================================================
void C_GeomComponent::Render(Renderer3D& renderer) const
{
	if (m_Pipeline.IsValid())
	{
		renderer.Draw(RenderCall3D{
			.ModelMatrix	= GetComponentModelMatrix(),
			.NumPrimities	= m_Mesh.m_NumPrimitives,
			.MaterialIndex	= m_Material->GetMaterialIndex(),
			.Buffers		= {m_Mesh.m_PositionsHandle, m_Mesh.m_NormalsHandle, m_Mesh.m_TexCoordsHandle, m_Mesh.m_TangentHandle, m_Mesh.m_BitangentHandle},
			.PipelineHandle = m_Pipeline,
		});
	}
}

//=================================================================================
Physics::Primitives::S_AABB C_GeomComponent::GetAABB() const
{
	return {glm::vec3{-1.f}, glm::vec3{1.f}};
}

//=================================================================================
void C_GeomComponent::DebugDrawGUI()
{
	m_Material->DrawGUI();
	rttr::instance obj(*this);
	if (GUI::DrawAllPropertyGUI(obj).empty() == false) {}
}

//=================================================================================
bool C_GeomComponent::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
void C_GeomComponent::AfterDeserialize(Utils::C_XMLDeserializer::DeserializeCtx& ctx)
{
	auto& materialManager = C_MaterialManager::Instance();
	materialManager.RegisterMaterial(m_Material);
}

} // namespace GLEngine::Renderer
