#pragma once

namespace GLEngine::Renderer::RG {
struct Resource {
	RGResourceType type;

	RGNodeHandle producer;
	RGResourceHandle outputHandle;

	std::uint_32t refCount = 0;

	std::string name; 
};
}