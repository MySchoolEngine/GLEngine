#pragma once

#include <Renderer/IRenderCommand.h>

#include <Core/CoreMacros.h>

#include <Utils/BitField.h>


namespace GLEngine::GLRenderer::Commands {

class C_GLClear : public Renderer::I_RenderCommand {
public:
	enum class E_ClearBits : std::uint8_t
	{
		Color	= 1,
		Depth	= 2,
		Accum	= 4, // actually I think this is not supported
		Stencil = 8,
	};

	explicit C_GLClear(E_ClearBits bits);
	explicit C_GLClear(::Utils::C_BitField<E_ClearBits> bits);

	//===========================================
	// Renderer::I_RenderCommand
	//===========================================
	virtual void								  Commit() override;
	virtual E_Type								  GetType() const override;
	[[nodiscard]] virtual std::string			  GetDescriptor() const override;

protected:
	int EnumToFlags(::Utils::C_BitField<E_ClearBits> bits) const;

	::Utils::C_BitField<E_ClearBits> m_Bits;
};

} // namespace GLEngine::GLRenderer::Commands

//=================================================================================
template <> struct Utils::enable_BitField_operators<GLEngine::GLRenderer::Commands::C_GLClear::E_ClearBits> {
	static constexpr bool enable = true;
};