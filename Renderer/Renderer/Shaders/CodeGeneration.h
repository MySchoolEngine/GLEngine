#pragma once

#include <string>

namespace GLEngine::Renderer::Shaders {

class I_CodeProvider {
public:
	virtual ~I_CodeProvider() = default;
	[[nodiscard]] virtual std::string GetStructCode(const std::string& name) const = 0;
};

} // namespace GLEngine::Renderer::Shaders
