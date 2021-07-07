#pragma once

#include <Renderer/RendererApi.h>

#include <GUI/LambdaPart.h>
#include <Core/GUID.h>

namespace GLEngine::GUI {
class C_GUIManager;
class C_LambdaPart;
} // namespace GLEngine::GUI

namespace GLEngine::Renderer {
class C_Material;
class RENDERER_API_EXPORT C_MaterialManager {
public:
	// Singleton stuff
	C_MaterialManager(C_MaterialManager const&)		= delete;
	void									operator=(C_MaterialManager const&) = delete;
	[[nodiscard]] static C_MaterialManager& Instance();

	[[nodiscard]] std::shared_ptr<C_Material> GetMaterial(const std::string& name);
	[[nodiscard]] std::shared_ptr<C_Material> RegisterMaterial(C_Material&& material);

	using T_MaterialEnumerator = std::function<void(std::shared_ptr<C_Material>&)>;
	void ForEachMaterial(const T_MaterialEnumerator& fn);

	GUID SetupControls(GUI::C_GUIManager& guiMGR);
	void DestroyControls(GUI::C_GUIManager& guiMGR);

private:
	C_MaterialManager();

	GUID									 m_Window;
	std::unique_ptr<GUI::C_LambdaPart>		 m_MaterialsList;
	std::vector<std::shared_ptr<C_Material>> m_Materials;
};
} // namespace GLEngine::Renderer