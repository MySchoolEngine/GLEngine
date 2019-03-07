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
template<class Extractor,	typename = std::enable_if<std::is_base_of<
								I_Extractor<typename Extractor>, typename Extractor
							>::value>::type
		>
class I_Resource {
public:
	virtual ~I_Resource() = default;

	virtual std::shared_ptr<I_RenderableComponent> GetComponent() const = 0;
	std::unique_ptr<I_RawGPUData> ExtractData() const
	{
		return Extractor::Extract(GetComponent());
	}

	virtual void Invalidate() = 0;
	virtual bool IsValid() = 0;
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