#include <CoreTestStdafx.h>

#include <CoreTest/Resources/TestClasses/TestCreatableResource.h>
#include <CoreTest/Resources/TestClasses/TestResource2.h>

namespace GLEngine::Core {

TEST(ResourceLoaderCapabilityTest, DefaultSupportsEmptyCreationIsFalse)
{
	TestResource2Loader loader;
	EXPECT_FALSE(loader.SupportsEmptyCreation());
}

TEST(ResourceLoaderCapabilityTest, OverriddenSupportsEmptyCreationIsTrue)
{
	TestCreatableResourceLoader loader;
	EXPECT_TRUE(loader.SupportsEmptyCreation());
}

TEST(ResourceLoaderCapabilityTest, GetResourceTypeNameMatchesResourceType)
{
	TestResource2Loader loader;
	EXPECT_EQ(loader.GetResourceTypeName(), TestResource2::GetResourceTypeName());
}
} // namespace GLEngine::Core
