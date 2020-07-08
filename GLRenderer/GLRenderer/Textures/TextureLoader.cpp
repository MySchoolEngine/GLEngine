#include <GLRendererStdafx.h>

#include <GLRenderer/Textures/TextureLoader.h>

#include <GLRenderer/Textures/Texture.h>

#include <gli/gli.hpp>


namespace GLEngine::GLRenderer::Textures {

//=================================================================================
std::shared_ptr<GLRenderer::Textures::C_Texture> TextureLoader::LoadAndInitTexture(const std::filesystem::path& path)
{
	gli::texture Texture = gli::load(path.generic_string());
	if (Texture.empty())
		return nullptr;


	gli::gl GL(gli::gl::PROFILE_GL33); // todo
	auto Format = GL.translate(Texture.format(), Texture.swizzles());
	GLenum Target = GL.translate(Texture.target());
	//GLenum internalFormat = GL.translate(Format.Internal, Texture.swizzles());

	GLE_ASSERT(Texture.target()== gli::TARGET_2D, "Only 2d supported now");
	auto texture = std::make_shared<Textures::C_Texture>(path.string());
	texture->StartGroupOp();
	texture->SetTexParameter(GL_TEXTURE_BASE_LEVEL, 0);
	texture->SetTexParameter(GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
	glTexParameteriv(Target, GL_TEXTURE_SWIZZLE_RGBA, &Format.Swizzles[0]);

	glTexStorage2D(Target, static_cast<GLint>(Texture.levels()), Format.Internal, Texture.extent(0).x, Texture.extent(0).y);

	for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
	{
		glm::tvec3<GLsizei> Extent(Texture.extent(Level));
		if (gli::is_compressed(Texture.format()))
		{
			glCompressedTexSubImage2D(
				Target, static_cast<GLint>(Level), 0, 0, Extent.x, Extent.y,
				Format.Internal, static_cast<GLsizei>(Texture.size(Level)), Texture.data(0, 0, Level));
		}
		else
		{
			glTexSubImage2D(
				Target, static_cast<GLint>(Level),
				0, 0,
				Extent.x,
				Extent.y,
				Format.External, Format.Type,
				Texture.data(0, 0, Level));
		}
	}

	texture->EndGroupOp();


	return texture;
}

}