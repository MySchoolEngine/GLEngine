#include <RendererStdAfx.h>

#include <Renderer/Materials/MaterialManager.h>

namespace GLEngine::Renderer {

//=================================================================================
C_MaterialManager::C_MaterialManager()
{

}

//=================================================================================
C_MaterialManager& C_MaterialManager::Instance()
{
	static C_MaterialManager    instance; // Guaranteed to be destroyed.
										// Instantiated on first use.
	return instance;
}

//=================================================================================
C_Material* C_MaterialManager::GetMaterial(const std::string& name)
{
	auto material = std::find_if(m_Materials.begin(), m_Materials.end(), [&name](const auto& mat) { return mat.GetName() == name; });
	if (material == m_Materials.end())
	{
		return nullptr;
	}

	return &(*material);
}

//=================================================================================
GLEngine::Renderer::C_Material& C_MaterialManager::RegisterMaterial(C_Material&& material)
{
	m_Materials.push_back(std::move(material));
	return m_Materials.back();
}

//=================================================================================
void C_MaterialManager::ForEachMaterial(const T_MaterialEnumerator& fn)
{
	std::for_each(m_Materials.begin(), m_Materials.end(), fn);
}

}
