#pragma once

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceLoader.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/Serialization/XMLSerialize.h>

#include <CoreTest/Resources/TestClasses/DelayTestResource.h>
#include <CoreTest/Resources/TestClasses/TestResource2.h>

namespace GLEngine::Core {

class TestResourceWithPropertyFile : public Resource {
public:
	DEFINE_RESOURCE_TYPE(TestResourceWithPropertyFile)

	TestResourceWithPropertyFile() = default;

	[[nodiscard]] bool Load(const std::filesystem::path& filepath, LoadCtx& ctx) override
	{
		m_Filepath = filepath;
		pugi::xml_document doc;

		pugi::xml_parse_result result = doc.load_file(m_Filepath.c_str());
		if (!result.status == pugi::status_ok)
		{
			CORE_LOG(E_Level::Error, E_Context::Core, "Can't open config file for trimesh name: {}", m_Filepath);
			return false;
		}
		Utils::C_XMLDeserializer d(ctx.m_ResMng);
		auto					 newTrimesh = d.Deserialize<std::shared_ptr<TestResourceWithPropertyFile>>(doc);
		if (newTrimesh.has_value() == false)
		{
			CORE_LOG(E_Level::Error, E_Context::Core, "XML {} is not valid TestResourceWithPropertyFile.", m_Filepath);
			return false;
		}

		return true;
	}

	[[nodiscard]] bool								Reload() override { return false; }
	[[nodiscard]] std::unique_ptr<I_ResourceLoader> GetLoader() override { return nullptr; }
	bool											SupportSaving() const override { return true; }


	// Observable result after loading.
	ResourceHandle<TestResource2> m_InnerHandle;

protected:
	bool SaveInternal() const override
	{
		Utils::C_XMLSerializer s;
		const auto			   str = s.Serialize(*this);
		return str.save_file(m_Filepath.c_str());
	}
};

class TestResourceWithPropertyFileLoader : public ResourceLoader<TestResourceWithPropertyFile> {
public:
	std::shared_ptr<Resource> CreateResource() const override;
	std::vector<std::string>  GetSupportedExtensions() const override;
};

} // namespace GLEngine::Core
