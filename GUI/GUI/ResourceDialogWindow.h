#pragma once

#include <GUI/FileDialogWindow.h>
#include <GUI/GUIManager.h>

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceManager.h>


namespace GLEngine::GUI {
template <Core::is_resource ResourceType> class C_ResourceDialogWindow final : public C_FileDialogWindow {
public:
	C_ResourceDialogWindow(std::reference_wrapper<Core::ResourceHandle<ResourceType>> resource, const std::string& windowName, const GUID& guid)
		: C_FileDialogWindow(GetResourcePaths(),
							 windowName,
							 std::bind(&C_ResourceDialogWindow::OnOk, this, std::placeholders::_1, std::placeholders::_2),
							 guid,
							 GetResourceDataPath())
		, m_Resource(resource)
	{}

private:
	static std::string GetResourcePaths()
	{
		auto&			   rm			= Core::C_ResourceManager::Instance();
		const auto		   resourceType = rttr::type::get<ResourceType>();
		const auto		   x			= resourceType.get_method("GetResourceTypeHashStatic").invoke({}).template get_value<std::size_t>();
		auto			   extensions	= rm.GetSupportedExtensions(x);
		std::ostringstream oss;
		std::ranges::copy(extensions, std::ostream_iterator<std::string>(oss, ", "));
		return oss.str();
	}

	static std::filesystem::path GetResourceDataPath()
	{
		if constexpr (requires { requires ResourceType::GetResourceDataPath() != ""; })
		{
			const auto resourceType = rttr::type::get<ResourceType>();
			if (auto getResourceDataPathMethod = resourceType.get_method("GetResourceDataPath"))
			{
				return getResourceDataPathMethod.invoke({}).template get_value<std::string_view>();
			}
			else
			{
				GLE_ERROR("Wrong resource registered, needs to use DEFINE_RESOURCE_WITH_PATH_TYPE macro instead");
			}
		}
		return "";
	}

	void OnOk(const std::filesystem::path& path, C_GUIManager& guiMGR)
	{
		auto& rm		 = Core::C_ResourceManager::Instance();
		m_Resource.get() = rm.LoadResource<ResourceType>(path);

		guiMGR.DestroyWindow(GetGuid());
	}

	std::reference_wrapper<Core::ResourceHandle<ResourceType>> m_Resource;
};
} // namespace GLEngine::GUI
