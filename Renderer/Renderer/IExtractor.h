#pragma once

#include <memory>

namespace GLEngine { namespace Renderer {

class I_RenderableComponent;
class I_RawGPUData;

template <class Extractor> class I_Extractor {
public:
	static std::unique_ptr<I_RawGPUData> Extract(std::shared_ptr<I_RenderableComponent> component) { return Extractor::ExtractInner(component); }
};

// leave it here for documentation now
/* class C_Extractor : public I_Extractor<C_Extractor> {
public:
	static std::unique_ptr<I_RawGPUData> ExtractInner(std::shared_ptr<I_RenderableComponent> component) {
		return nullptr;
	}
};*/

}} // namespace GLEngine::Renderer
