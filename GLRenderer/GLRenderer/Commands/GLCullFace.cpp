#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/GLCullFace.h>

namespace GLEngine { namespace GLRenderer { namespace Commands {

//=================================================================================
C_GLCullFace::C_GLCullFace(E_FaceMode mode)
	: m_Mode(mode)
{
}

//=================================================================================
void C_GLCullFace::Commit()
{
	glCullFace(EnumCast(m_Mode));
}

//=================================================================================
Renderer::I_RenderCommand::E_Type C_GLCullFace::GetType() const
{
	return Renderer::I_RenderCommand::E_Type::StateChange;
}

//=================================================================================
std::shared_ptr<Renderer::I_Resource> C_GLCullFace::GetResource() const
{
	return nullptr;
}

//=================================================================================
std::string C_GLCullFace::GetDescriptor() const
{
	return "CullFace";
}

//=================================================================================
GLenum C_GLCullFace::EnumCast(E_FaceMode mode)
{
	static GLenum const Translate[] = {
		/*[static_cast<std::size_t>(C_GLCullFace::E_FaceMode::Back)] =*/GL_BACK,
		/*[static_cast<std::size_t>(E_FaceMode::Back)] =*/GL_FRONT,
		/*[static_cast<std::size_t>(E_FaceMode::Back)] =*/GL_FRONT_AND_BACK,
	};
	static_assert(sizeof(Translate) / sizeof(std::uint32_t) == static_cast<std::size_t>(C_GLCullFace::E_FaceMode::Last) + 1,
				  "OPENGL ERROR: The translation table for 'face culling' needs to be updated.");

	return Translate[static_cast<std::underlying_type<E_FaceMode>::type>(mode)];
}

}}} // namespace GLEngine::GLRenderer::Commands