#pragma once

#include <Renderer/RendererApi.h>

// A component TU whose structs are entirely header-defined has nothing else outside the TU that
// references an out-of-line symbol from it - in a static-library link (e.g. RendererTest on MSVC
// links RendererStatic, not the Renderer DLL), the linker then drops that whole object file as
// unused, and its RTTR_REGISTRATION/C_ComponentRegistry static initializers never run. Declaring
// and calling one such function per TU forces it to be linked in, which is sufficient for the
// rest of that TU's static initializers to run too. Not needed in the real Renderer.dll build,
// where every linked TU's static init runs unconditionally at load time.
#define DECLARE_FORCE_LINK_COMPONENT_TU(FunctionName) RENDERER_API_EXPORT void FunctionName()
#define DEFINE_FORCE_LINK_COMPONENT_TU(FunctionName) \
	void FunctionName() {}
