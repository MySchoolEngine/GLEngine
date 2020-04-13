#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/EntitiesWindow.h>

#include <GLRenderer/Entities/TerrainEntity.h>
#include <GLRenderer/Components/TerrainMesh.h>
#include <GLRenderer/Textures/TextureLoader.h>
#include <GLRenderer/Textures/Texture.h>

#include <Entity/EntityManager.h>
#include <Entity/BasicEntity.h>


namespace GLEngine::GLRenderer::GUI {

//=================================================================================
C_EntitiesWindow::C_EntitiesWindow(GUID guid, std::shared_ptr<Entity::C_EntityManager>& world)
	: C_Window(guid, "Entities")
	, m_World(world)
{}

//=================================================================================
C_EntitiesWindow::~C_EntitiesWindow()
{
	CORE_LOG(E_Level::Info, E_Context::Render, "~C_EntitiesWindow()");
}

//=================================================================================
void C_EntitiesWindow::Draw() const
{
	::ImGui::Begin(m_Name.c_str(), &m_IsVisible);
	if (::ImGui::Button("Spawn new terrain")) {
		auto Terrain = std::make_shared<C_TerrainEntity>("Terrain");
		m_World->AddEntity(Terrain);
		Terrain->AddPatch(glm::ivec2(0, 0));
	}
	for (const auto& entity : m_World->GetEntities()) {
		bool selected = false;
		::ImGui::Selectable(entity->GetName().c_str(), &selected);
		if (selected) {
			entity->OnEvent(Core::C_UserEvent("selected"));
		}
	}
	::ImGui::End();
}

}