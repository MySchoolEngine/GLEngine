#pragma once

#include <GLRenderer/Buffers/UniformBuffer.h>
#include <GLRenderer/Lights/AreaLightStruct.h>
#include <GLRenderer/Lights/PointLightStruct.h>

#include <GLRenderer\Lights\PointLightStruct.h>
#include <GLRenderer\Lights\AreaLightStruct.h>
#include <GLRenderer\Lights\SunLigth.h>

namespace GLEngine::GLRenderer {
namespace Textures {
class C_Texture;
}

class C_LightsBuffer final : public Buffers::C_UniformBuffer {
public:
	C_LightsBuffer(const std::string& blockName, unsigned int index);
	virtual void UploadData() const override;

	void SetPointLight(S_PointLight pl, std::size_t index) { m_PointLight[index] = pl; }

	[[nodiscard]] std::size_t PointLightsLimit() const { return m_PointLight.max_size(); }

	void					  SetAreaLight(S_AreaLight light, std::size_t index) { m_AreaLight[index] = light; }
	[[nodiscard]] std::size_t AreaLightsLimit() const { return m_AreaLight.max_size(); }

	S_SunLight& GetSunLight() { return m_SunLight; }

	void MakeHandlesResident(bool val = true);

private:
	std::array<S_PointLight, 10>	m_PointLight;
	std::array<S_AreaLight, 4>		m_AreaLight;
	S_SunLight						m_SunLight;
	std::uint64_t					m_LTCFittingSchemeHandle;
	std::uint64_t					m_LTCMagSchemeHandle;

	// do not upload this
	std::shared_ptr<Textures::C_Texture> m_LTCFittingTexture;
	std::shared_ptr<Textures::C_Texture> m_LTCMagTexture;
};
} // namespace GLEngine::GLRenderer
