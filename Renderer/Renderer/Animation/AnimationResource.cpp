#include <RendererStdafx.h>

#include <Renderer/Animation/AnimationResource.h>

namespace GLEngine::Renderer {

//=================================================================================
bool AnimationResource::Load(const std::filesystem::path& filepath)
{
	return false;
}

//=================================================================================
bool AnimationResource::Reload()
{
	return false;
}

//=================================================================================
std::shared_ptr<Core::Resource> AnimationLoader::CreateResource() const
{
	return std::make_shared<AnimationResource>();
}

//=================================================================================
std::vector<std::string> AnimationLoader::GetSupportedExtensions() const
{
	return {".dae"};
}

}
