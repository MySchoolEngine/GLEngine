#pragma once

#include <GLRenderer\Buffers\UniformBuffer.h>

#include <GLRenderer\Lights\PointLightStruct.h>
#include <GLRenderer\Lights\AreaLightStruct.h>

namespace GLEngine::GLRenderer {
class C_LightsBuffer final : public Buffers::C_UniformBuffer {
public:
	C_LightsBuffer(const std::string& blockName, unsigned int index);
	virtual void UploadData() const override;

	void SetPointLight(S_PointLight pl, std::size_t index) { m_PointLight[index] = pl; }

	[[nodiscard]] std::size_t PointLightsLimit() const { return m_PointLight.max_size(); }

	void SetAreaLight(S_AreaLight light, std::size_t index) { m_AreaLight[index] = light; }
	[[nodiscard]] std::size_t AreaLightsLimit() const { return m_AreaLight.max_size(); }
private:
	std::array<S_PointLight, 10>	m_PointLight;
	std::array<S_AreaLight, 4>		m_AreaLight;
};
}
