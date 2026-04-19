#include <RendererTestStdafx.h>

#include <Renderer/Components/StaticMeshHandles.h>
#include <Renderer/Materials/Material.h>

#include <pugixml.hpp>

#include <CommonTestUtils/XMLDeserializeFixture.h>

namespace GLEngine::Renderer {

// disabled because C_StaticMeshHandles registration is not exported
TEST_F(XMLDeserializeFixture, DISABLED_LoadMaterial)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<C_StaticMeshHandles>
	<Transformation Translation="(0.0,-1.5,0.0)" />
	<Material Name="None" Color="(1.0,1.0,1.0)" Roughness="0.5" Shininess="500">
	</Material>
</C_StaticMeshHandles>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<C_StaticMeshHandles>>(doc);
	EXPECT_TRUE(result.has_value());
}

// disabled because C_StaticMeshHandles registration is not exported
TEST_F(XMLDeserializeFixture, DISABLED_LoadMaterial_DefaultTransform)
{
	RecordProperty("description", "Transformation has default value that should be applied if tag is missing");
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<C_StaticMeshHandles>
	<Material Name="None" Color="(1.0,1.0,1.0)" Roughness="0.5" Shininess="500">
	</Material>
</C_StaticMeshHandles>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<C_StaticMeshHandles>>(doc);
	EXPECT_TRUE(result.has_value());
}
} // namespace GLEngine::Renderer