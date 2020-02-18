#include <GLRendererStdafx.h>

#include <GLRenderer/Components/ComponentBuilderFactory.h>

#include <GLRenderer/Components/SkyBox.h>
#include <GLRenderer/Components/StaticMesh.h>
#include <GLRenderer/Components/SkeletalMesh.h>
#include <GLRenderer/GUI/Components/GLEntityDebugComponent.h>

namespace GLEngine::GLRenderer::Components {

//=================================================================================
std::unique_ptr<Entity::I_ComponenetBuilder> C_ComponentBuilderFactory::GetFactory(const std::string& name)
{
	if (name == "SkyBox")
	{
		return std::make_unique<C_SkyBoxCompBuilder>();
	}
	if (name == "staticMesh")
	{
		return std::make_unique<C_StaticMeshBuilder>();
	}
	if (name == "debug")
	{
		return std::make_unique<GUI::C_GUIDebugBuilder>();
	}
	if (name == "SkeletalMesh")
	{
		return std::make_unique<C_SkeletalMeshBuilder>();
	}
	return nullptr;
}

}