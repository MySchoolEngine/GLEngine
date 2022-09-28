#pragma once

namespace GLEngine::Renderer {

class I_RenderCommand {
public:
	virtual ~I_RenderCommand() = default;
	enum class E_Type
	{
		StateChange,
		DrawCall,
		Undefined,
	};
	virtual void					  Commit()				= 0;
	[[nodiscard]] virtual E_Type	  GetType() const		= 0;
	[[nodiscard]] virtual std::string GetDescriptor() const = 0;
};

} // namespace GLEngine::Renderer