#pragma once

#include <Renderer/IRenderCommand.h>

#include <DULib/BitField.h>


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
	explicit C_GLClear(DULib::BitField<E_ClearBits> bits);

	//===========================================
	// Renderer::I_RenderCommand
	//===========================================
	void					  Commit() override;
	E_Type					  GetType() const override;
	[[nodiscard]] std::string GetDescriptor() const override;

protected:
	int EnumToFlags(DULib::BitField<E_ClearBits> bits) const;

	DULib::BitField<E_ClearBits> m_Bits;
};

} // namespace GLEngine::GLRenderer::Commands

//=================================================================================
template <> struct DULib::enable_BitField_operators<GLEngine::GLRenderer::Commands::C_GLClear::E_ClearBits> {
	static constexpr bool enable = true;
};

template <> struct DULib::BitField_UsedBitsCounter<GLEngine::GLRenderer::Commands::C_GLClear::E_ClearBits> {
	static constexpr std::size_t usedBits = 4;
};