#pragma once

#include <GLRenderer\Buffers\UniformBuffer.h>

#include <GLRenderer\Lights\PointLightStruct.h>

namespace GLEngine::GLRenderer {
class C_LightsBuffer final : public Buffers::C_UniformBuffer {
public:
	C_LightsBuffer(const std::string& blockName, unsigned int index);
	virtual void UploadData() const override;

private:
	S_PointLight m_PointLight;
};
}
