#pragma once

#include <Renderer/Materials/Material.h>
#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer
{
class RENDERER_API_EXPORT C_MaterialManager
{
public:
	//Singleton stuff
	C_MaterialManager(C_MaterialManager const&) = delete;
	void operator=(C_MaterialManager const&) = delete;
	[[nodiscard]] static C_MaterialManager& Instance();

	[[nodiscard]] C_Material* GetMaterial(const std::string& name);
	[[nodiscard]] C_Material& RegisterMaterial(C_Material&& material);

	using T_MaterialEnumerator = std::function<void(C_Material&)>;
	void ForEachMaterial(const T_MaterialEnumerator& fn);
private:
	C_MaterialManager();

	std::vector<C_Material> m_Materials;
};
}