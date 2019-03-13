#pragma once

#include <Renderer/IExtractor.h>
#include <Renderer/IRawGPUData.h>

#include <memory>
#include <type_traits>

namespace GLEngine {
namespace Renderer {

class I_RenderableComponent;

/** ==============================================
 * @class I_Resource
 *
 * @brief This class represents data for GPU. It encapsulate way how to extract them form @I_Entity
 *		  and their state in GPU memory.
 *
 * @author 	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * @date 	2019/03/06
 ** ==============================================*/
class I_Resource {
public:
	virtual ~I_Resource() = default;

	inline std::shared_ptr<I_RenderableComponent> GetComponent() const { return m_Component; };
	inline void SetComponent(std::shared_ptr<I_RenderableComponent> component) { m_Component = component; }
	virtual std::unique_ptr<I_RawGPUData> ExtractData() const = 0;

	virtual void Invalidate() = 0;
	virtual bool IsValid() = 0;

protected:
	std::shared_ptr<I_RenderableComponent> m_Component;
};

// leave it here for documentation
/*class C_SkeletonResource : I_Resource<C_Extractor> {

	virtual std::shared_ptr<I_RenderableComponent> GetComponent() const override { return nullptr; };
	virtual void Invalidate() override {};
	virtual bool IsValid() override { return false; };
};*/

}
}

#include <Renderer/IResource.inl>