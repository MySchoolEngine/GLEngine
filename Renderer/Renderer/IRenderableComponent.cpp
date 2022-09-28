#include <RendererStdafx.h>

#include <Renderer/IRenderableComponent.h>

namespace GLEngine::Renderer {

RTTR_REGISTRATION
{
	rttr::registration::class_<I_RenderableComponent>("I_RenderableComponent");
}
}
