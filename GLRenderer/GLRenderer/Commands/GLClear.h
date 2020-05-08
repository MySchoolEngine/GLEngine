#pragma once

#include <Core/CoreMacros.h>

#include <Renderer/IRenderCommand.h>

#include <Utils/BitField.h>

namespace GLEngine {
namespace GLRenderer {
namespace Commands {

class C_GLClear : public Renderer::I_RenderCommand {
public:
	enum class E_ClearBits : std::uint8_t {
		Color = 1,
		Depth = 2,
		Accum = 4, // actually I think this is not supported
		Stencil = 8,
	};

	explicit C_GLClear(E_ClearBits bits);
	explicit C_GLClear(Utils::C_BitField<E_ClearBits>  bits);

	//===========================================
	// Renderer::I_RenderCommand
	//===========================================
	virtual void Commit() override;
	virtual E_Type GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override;


protected:
	int EnumToFlags(Utils::C_BitField<E_ClearBits> bits) const;

	Utils::C_BitField<E_ClearBits> m_Bits;
};

}}}

//=================================================================================
template<>
struct Utils::enable_BitField_operators<GLEngine::GLRenderer::Commands::C_GLClear::E_ClearBits> {
	static constexpr bool enable = true;
};