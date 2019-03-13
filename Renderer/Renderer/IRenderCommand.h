#pragma once

#include <Renderer/IResource.h>

#include <memory>

namespace GLEngine {
namespace Renderer {

class I_RenderCommand {
public:
	virtual ~I_RenderCommand() = default;
	enum class E_Type {
		StateChange,
		DrawCall,
		Undefined,
	};
	virtual void Commit() = 0;
	virtual E_Type GetType() const = 0;

	virtual std::shared_ptr<I_Resource> GetResource() const = 0;
};

}
}