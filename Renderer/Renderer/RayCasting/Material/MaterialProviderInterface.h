#pragma once

#include <Renderer/Materials/MaterialResource.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/RayCasting/Material/MaterialInterface.h>

#include <Core/Resources/ResourceHandle.h>

namespace GLEngine::Renderer {
class I_MaterialProviderInterface {
public:
	I_MaterialProviderInterface()		   = default;
	virtual ~I_MaterialProviderInterface() = default;

	virtual I_MaterialInterface* AddMaterial(const Core::ResourceHandle<MaterialResource>& material) = 0;
	virtual I_MaterialInterface* AddMaterial(const MeshData::Material& material)					 = 0;
};
} // namespace GLEngine::Renderer