#pragma once

#include <Renderer/Descriptors/TextureDescriptor.h>

#include <glm/glm.hpp>

namespace GLEngine::Renderer {
class I_TextureSampler2D {
public:
	I_TextureSampler2D(const SamplerDescriptor2D& desc)
		: m_Desc(desc)
	{
	}

	virtual void SetWrap(Renderer::E_WrapFunction wrapS, Renderer::E_WrapFunction wrapT) = 0;
	virtual void SetFilter(Renderer::E_TextureFilter min, Renderer::E_TextureFilter mag) = 0;
	virtual void SetBorderColor(const glm::vec4& color)									 = 0;

protected:
	SamplerDescriptor2D m_Desc;
};
} // namespace GLEngine::Renderer