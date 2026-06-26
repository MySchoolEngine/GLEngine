#pragma once

#include <Renderer/RendererApi.h>

#include <rttr/type.h>

namespace GLEngine::Renderer {

class RENDERER_API_EXPORT I_MaterialData {
public:
	virtual ~I_MaterialData() = default;

	// Returns true if any property changed
	virtual bool DrawGUI() = 0;

	RTTR_ENABLE()
};

} // namespace GLEngine::Renderer
