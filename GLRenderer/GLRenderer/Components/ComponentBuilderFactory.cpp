#include <GLRendererStdafx.h>

#include <GLRenderer/Components/ComponentBuilderFactory.h>

namespace GLEngine::GLRenderer::Components {

//=================================================================================
I_ComponenetBuilder* C_ComponentBuilderFactory::GetFactory(const std::string& name)
{
	if (name == "SkyBox")
	{
		return nullptr;
	}
}

}