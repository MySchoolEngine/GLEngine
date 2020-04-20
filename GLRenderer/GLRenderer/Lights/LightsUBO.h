#pragma once

#include <GLRenderer\Buffers\UniformBuffer.h>

#include <GLRenderer\Lights\PointLightStruct.h>

namespace GLEngine::GLRenderer {
class C_LightsBuffer final : public Buffers::C_UniformBuffer {
public:
	C_LightsBuffer(const std::string& blockName, unsigned int index);
	virtual void UploadData() const override;

	void SetPointLight(S_PointLight pl, std::size_t index) { m_PointLight[index] = pl; }

	std::size_t PointLightsLimit() const { return m_PointLight.max_size(); }

private:
	std::array<S_PointLight, 10> m_PointLight;
};
}
