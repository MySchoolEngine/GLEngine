#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceManager.h>

#include <CoreTest/Resources/Fixtures/ResourceManagerBaseFixture.h>
#include <CoreTest/Resources/TestClasses/DelayTestResource.h>
#include <CoreTest/Resources/TestClasses/TestResourceWithHandleContainers.h>

// Regression coverage for a bug where C_XMLDeserializer::DeserializeArray called
// FinishDeserialization() (which runs AfterDeserialize(), the thing that actually
// loads a ResourceHandle's resource) *after* the element had already been copied
// into the destination container. Every element ended up in the container with
// its resource still null; the loaded resource only ever existed on the local
// temporary that got discarded at the next loop iteration. DeserializeAssociativeArray
// already called FinishDeserialization() before insertion, so the map case was
// never affected -- it's tested here anyway as a locked-in regression alongside
// the vector case.

namespace GLEngine::Core {

class ResourceHandleContainerFixture : public ResourceManagerBaseFixture {
public:
	static inline const std::filesystem::path s_TestFilepath{"ResourceHandleContainerFixture"};

	void SetUp() override
	{
		ResourceManagerBaseFixture::SetUp();
		std::filesystem::create_directory(s_TestFilepath);
		auto& manager = C_ResourceManager::Instance();
		manager.RegisterResourceType(new DelayTestResourceLoader);
		manager.RegisterResourceType(new TestResourceWithHandleContainersLoader);
	}

	void TearDown() override
	{
		RemoveMetafileIfExists(s_TestFilepath / "containers.handlecontainers");
		for (const auto* elementPath : {"handle_vector_0.test", "handle_vector_1.test", "handle_map_0.test", "handle_map_1.test"})
			RemoveMetafileIfExists(elementPath);
		ResourceManagerBaseFixture::TearDown();
	}
};

TEST_F(ResourceHandleContainerFixture, SequentialContainerElementsAreLoadedAfterDeserialize)
{
	auto& manager	= C_ResourceManager::Instance();
	const auto filePath = s_TestFilepath / "containers.handlecontainers";
	DeleteOnTearDown(filePath);

	auto resource = std::make_shared<TestResourceWithHandleContainers>();
	resource->m_HandleVector.push_back(manager.LoadResource<DelayTestResource>("handle_vector_0.test", true));
	resource->m_HandleVector.push_back(manager.LoadResource<DelayTestResource>("handle_vector_1.test", true));
	SetFilePath(resource, filePath);
	SetDirty(resource);
	ASSERT_TRUE(resource->Save());

	const auto loaded = manager.LoadResource<TestResourceWithHandleContainers>(filePath, true);
	ASSERT_TRUE(loaded.IsReady());

	const auto& loadedResource = loaded.GetResource();
	ASSERT_EQ(loadedResource.m_HandleVector.size(), 2u);
	EXPECT_TRUE(loadedResource.m_HandleVector[0].IsReady()) << "First vector element's resource must be loaded, not left null on a discarded temporary";
	EXPECT_TRUE(loadedResource.m_HandleVector[1].IsReady()) << "Second vector element's resource must be loaded, not left null on a discarded temporary";
}

TEST_F(ResourceHandleContainerFixture, AssociativeContainerValuesAreLoadedAfterDeserialize)
{
	auto& manager	= C_ResourceManager::Instance();
	const auto filePath = s_TestFilepath / "containers.handlecontainers";
	DeleteOnTearDown(filePath);

	auto resource				= std::make_shared<TestResourceWithHandleContainers>();
	resource->m_HandleMap[0]	= manager.LoadResource<DelayTestResource>("handle_map_0.test", true);
	resource->m_HandleMap[1]	= manager.LoadResource<DelayTestResource>("handle_map_1.test", true);
	SetFilePath(resource, filePath);
	SetDirty(resource);
	ASSERT_TRUE(resource->Save());

	const auto loaded = manager.LoadResource<TestResourceWithHandleContainers>(filePath, true);
	ASSERT_TRUE(loaded.IsReady());

	const auto& loadedResource = loaded.GetResource();
	ASSERT_EQ(loadedResource.m_HandleMap.size(), 2u);
	ASSERT_TRUE(loadedResource.m_HandleMap.contains(0));
	ASSERT_TRUE(loadedResource.m_HandleMap.contains(1));
	EXPECT_TRUE(loadedResource.m_HandleMap.at(0).IsReady()) << "Map value at key 0 must be loaded, not left null on a discarded temporary";
	EXPECT_TRUE(loadedResource.m_HandleMap.at(1).IsReady()) << "Map value at key 1 must be loaded, not left null on a discarded temporary";
}

} // namespace GLEngine::Core
