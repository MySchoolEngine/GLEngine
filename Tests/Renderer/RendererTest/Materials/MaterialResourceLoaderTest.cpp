#include <RendererTestStdafx.h>

#include <Renderer/Materials/MaterialResource.h>

namespace GLEngine::Renderer {

TEST(MaterialResourceLoaderTest, SupportsEmptyCreationIsTrue)
{
	MaterialResourceLoader loader;
	EXPECT_TRUE(loader.SupportsEmptyCreation());
}

TEST(MaterialResourceLoaderTest, CreateResourceProducesMaterialWithDefaultData)
{
	MaterialResourceLoader loader;
	const auto				resource = loader.CreateResource();
	ASSERT_NE(resource, nullptr);

	const auto material = std::dynamic_pointer_cast<MaterialResource>(resource);
	ASSERT_NE(material, nullptr);
	EXPECT_NE(material->GetMaterialData(), nullptr);
}
} // namespace GLEngine::Renderer
