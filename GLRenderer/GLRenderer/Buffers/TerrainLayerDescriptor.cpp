#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/TerrainLayerDescriptor.h>


namespace GLEngine::GLRenderer {

//=================================================================================
Shaders::C_StructDescriptor S_TerrainLayerDescriptor::GetDescription() const
{
	Shaders::C_StructDescriptor ret("pointLight");
	ret.Push({ "Color", "vec3" });
	ret.Push({ "ThicknessFactor", "float" });
	ret.Push({ "Granularity", "float" });
	return ret;
}

//=================================================================================
std::string S_TerrainLayerDescriptor::GetName() const
{
	return "S_TerrainLayerDescriptor";
}
}