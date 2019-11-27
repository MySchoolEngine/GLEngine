#include <GLRendererStdafx.h>

#include <GLRenderer/Components/ComponentBuilderFactory.h>

#include <GLRenderer/Components/SkyBox.h>

namespace GLEngine::GLRenderer::Components {

//=================================================================================
std::unique_ptr<Entity::I_ComponenetBuilder> C_ComponentBuilderFactory::GetFactory(const std::string& name)
{
	if (name == "SkyBox")
	{
		return std::make_unique<C_SkyBoxCompBuilder>();
	}
	return nullptr;
}

}