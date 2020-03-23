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
	, m_Spawning(false)
	, m_SpawningName("")
	, m_SpawningFilename("")
{

}

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
		m_Spawning = true;
		m_SpawningName[0] = '\0';
		m_SpawningFilename[0] = '\0';
	}
	for (const auto& entity : m_World->GetEntities()) {
		bool selected = false;
		::ImGui::Selectable(entity->GetName().c_str(), &selected);
		if (selected) {
			entity->OnEvent(Core::C_UserEvent("selected"));
		}
	}
	::ImGui::End();

	if (m_Spawning) {
		::ImGui::Begin("Spawn terrain", &m_Spawning);
		::ImGui::InputText("Terrain name", m_SpawningName, 255);
		::ImGui::InputText("Terrain filename", m_SpawningFilename, 255);

		if (::ImGui::Button("Spawn")) {
			m_Spawning = false;
			auto Terrain = std::make_shared<C_TerrainEntity>(m_SpawningName);
			m_World->AddEntity(Terrain);

			if (std::string_view(m_SpawningFilename).empty())
			{
				Terrain->AddPatch(glm::ivec2(0, 0));
			}
			else
			{
				Textures::TextureLoader tl;
				Renderer::MeshData::Texture t;
				bool retval = tl.loadTexture(m_SpawningFilename, t);
				if (retval) {
					Textures::C_Texture ct(m_SpawningFilename);

					ct.StartGroupOp();
					ct.SetTexData2D(0, t);
					ct.SetWrap(E_WrapFunction::Repeat, E_WrapFunction::Repeat);
					ct.SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
					ct.GenerateMipMaps();

					ct.EndGroupOp();

					auto patch = std::make_shared<Components::C_TerrainMesh>(std::move(ct));
					Terrain->AddPatch(patch);
					patch->SetCoord({ 0,0 });

				}
				else {
					CORE_LOG(E_Level::Error, E_Context::Entity, "Given texture {} doesn't exists", m_SpawningFilename);
					Terrain->AddPatch(glm::ivec2(0, 0));
				}
			}
		}

		::ImGui::End();
	}
}

}