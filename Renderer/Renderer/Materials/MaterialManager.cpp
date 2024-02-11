#include <RendererStdafx.h>

#include <Renderer/Materials/Material.h>
#include <Renderer/Materials/MaterialManager.h>

#include <GUI/GUIManager.h>
#include <GUI/GUIWindow.h>

#include <Utils/Serialization/XMLSerialize.h>

#include <imgui.h>

namespace GLEngine::Renderer {

//=================================================================================
C_MaterialManager::C_MaterialManager()
{
}

//=================================================================================
C_MaterialManager& C_MaterialManager::Instance()
{
	static C_MaterialManager instance; // Guaranteed to be destroyed.
									   // Instantiated on first use.
	return instance;
}

//=================================================================================
std::shared_ptr<C_Material> C_MaterialManager::GetMaterial(const std::string& name)
{
	auto material = std::find_if(m_Materials.begin(), m_Materials.end(), [&name](const auto& mat) { return mat->GetName() == name; });
	if (material == m_Materials.end())
	{
		return nullptr;
	}

	return *material;
}

//=================================================================================
std::shared_ptr<const C_Material> C_MaterialManager::GetMaterial(const std::string& name) const
{
	auto material = std::find_if(m_Materials.begin(), m_Materials.end(), [&name](const auto& mat) { return mat->GetName() == name; });
	if (material == m_Materials.end())
	{
		return nullptr;
	}

	return *material;
}

//=================================================================================
std::shared_ptr<C_Material> C_MaterialManager::RegisterMaterial(C_Material&& material)
{
	m_Materials.push_back(std::make_shared<C_Material>(std::move(material)));
	m_Materials.back()->SetMaterialIndex(static_cast<int>(m_Materials.size()) - 1);
	return m_Materials.back();
}

//=================================================================================
void C_MaterialManager::UnregisterMaterial(std::shared_ptr<C_Material>& material)
{
	const int matIndex = material->GetMaterialIndex();
	m_Materials.erase(m_Materials.begin() + matIndex);
	for (int i = 0; i < m_Materials.size(); ++i) {
		m_Materials[i]->m_Changed = true;
		m_Materials[i]->SetMaterialIndex(i);
	}
}

//=================================================================================
void C_MaterialManager::ForEachMaterial(const T_MaterialEnumerator& fn)
{
	std::for_each(m_Materials.begin(), m_Materials.end(), [&fn](std::shared_ptr<C_Material>& mat) {
		if (mat)
		{
			fn(*mat.get());
		}
	});
}

//=================================================================================
GUID C_MaterialManager::SetupControls(GUI::C_GUIManager& guiMGR)
{
	m_Window		  = guiMGR.CreateGUIWindow("Material manager");
	auto* materialMan = guiMGR.GetWindow(m_Window);

	m_MaterialsList = std::make_unique<GUI::C_LambdaPart>([&]() {
		bool changed = false;
		for (const auto& material : m_Materials)
		{
			if (::ImGui::CollapsingHeader(material->GetName().c_str()))
			{
				changed |= material->DrawGUI();
				if (::ImGui::Button("Save"))
				{
					SaveMaterial(material->GetName(), std::filesystem::path("Materials") / (material->GetName() + ".xml"));
				}
				// todo if changed should register clone
			}
		}
		return changed;
	});

	materialMan->AddComponent(*m_MaterialsList.get());

	return m_Window;
}

//=================================================================================
void C_MaterialManager::DestroyControls(GUI::C_GUIManager& guiMGR)
{
	guiMGR.DestroyWindow(m_Window);
}

//=================================================================================
void C_MaterialManager::SaveMaterial(const std::string& name, const std::filesystem::path& path) const
{
	Utils::C_XMLSerializer s;
	const auto			   materialToSave = GetMaterial(name);
	const auto			   str			  = s.Serialize(materialToSave);
	str.save_file(path.c_str());
}

//=================================================================================
void C_MaterialManager::Update()
{
	ForEachMaterial([](C_Material& mat) { mat.Update(); });
}

} // namespace GLEngine::Renderer
