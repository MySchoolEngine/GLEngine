#pragma once

#include <Renderer/IResource.h>

namespace GLEngine::GLRenderer::Shaders {

/** ==============================================
 * @class C_ShaderResource
 *
 * @brief This class is created by shader manager. Also shader manager holds this resource.
 *
 * @author 	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * @date 	2019/03/19
 ** ==============================================*/
class C_ShaderResource : public Renderer::I_Resource {
public:
	C_ShaderResource(const std::string& filename);

	//=================================================================================
	virtual std::unique_ptr<Renderer::I_RawGPUData> ExtractData() const override;
	virtual void									Invalidate() override;
	virtual bool									IsValid() override;

private:
};

} // namespace GLEngine::GLRenderer::Shaders