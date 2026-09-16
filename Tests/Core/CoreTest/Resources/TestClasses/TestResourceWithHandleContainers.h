#pragma once

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceLoader.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/Serialization/XMLSerialize.h>

#include "Utils/Serialization/XMLDeserialize.h"
#include <CoreTest/Resources/TestClasses/DelayTestResource.h>
#include <map>
#include <vector>

namespace GLEngine::Core {

// A resource whose properties are a sequential and an associative container of
// ResourceHandle<DelayTestResource>. Exists to regression-test that every
// element deserialized into such a container is actually loaded (AfterDeserialize
// ran on the same object that ended up in the container), not left behind on a
// discarded temporary. See C_XMLDeserializer::DeserializeArray /
// DeserializeAssociativeArray.
class TestResourceWithHandleContainers : public Resource {
public:
	DEFINE_RESOURCE_TYPE(TestResourceWithHandleContainers)

	TestResourceWithHandleContainers() = default;

	[[nodiscard]] bool Load(const std::filesystem::path& filepath, LoadCtx& ctx) override
	{
		m_Filepath = filepath;
		pugi::xml_document doc;

		pugi::xml_parse_result result = doc.load_file(m_Filepath.c_str());
		if (result.status != pugi::status_ok)
		{
			CORE_LOG(E_Level::Error, E_Context::Core, "Can't open config file for TestResourceWithHandleContainers: {}", m_Filepath);
			return false;
		}
		Utils::C_XMLDeserializer d(ctx.m_ResMng, ctx.m_Query, ctx.m_isBlocking);
		auto					 loaded = d.Deserialize<std::shared_ptr<TestResourceWithHandleContainers>>(doc);
		if (!loaded.has_value())
		{
			CORE_LOG(E_Level::Error, E_Context::Core, "XML {} is not valid TestResourceWithHandleContainers.", m_Filepath);
			return false;
		}

		m_HandleVector = std::move(loaded->get()->m_HandleVector);
		m_HandleMap	   = std::move(loaded->get()->m_HandleMap);
		return true;
	}

	[[nodiscard]] bool								Reload() override { return false; }
	[[nodiscard]] std::unique_ptr<I_ResourceLoader> GetLoader() override { return nullptr; }
	bool											SupportSaving() const override { return true; }

	// Observable result after loading.
	std::vector<ResourceHandle<DelayTestResource>>   m_HandleVector;
	std::map<int, ResourceHandle<DelayTestResource>> m_HandleMap;

protected:
	bool SaveInternal() const override
	{
		Utils::C_XMLSerializer s;
		const auto				str = s.Serialize(*this);
		return str.save_file(m_Filepath.c_str());
	}
};

class TestResourceWithHandleContainersLoader : public ResourceLoader<TestResourceWithHandleContainers> {
public:
	std::shared_ptr<Resource> CreateResource() const override;
	std::vector<std::string>  GetSupportedExtensions() const override;
};

} // namespace GLEngine::Core
