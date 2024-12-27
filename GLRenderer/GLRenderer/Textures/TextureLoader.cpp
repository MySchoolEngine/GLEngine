#include <GLRendererStdafx.h>

#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureLoader.h>

#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Textures/TextureDefinitions.h>

#include <Core/Application.h>

#pragma warning(push)
#pragma warning(disable : 4005)
#include <gli/gli.hpp>
#pragma warning(pop)


namespace GLEngine::GLRenderer::Textures {

//=================================================================================
std::shared_ptr<C_Texture> TextureLoader::LoadAndInitTexture(const std::filesystem::path& path)
{
	gli::texture Texture = gli::load(path.generic_string());
	if (Texture.empty())
		return nullptr;


	gli::gl GL(gli::gl::PROFILE_GL33); // todo
	auto	Format = GL.translate(Texture.format(), Texture.swizzles());
	GLenum	Target = GL.translate(Texture.target());
	// GLenum internalFormat = GL.translate(Format.Internal, Texture.swizzles());

	// should be already allocated
	GLE_ASSERT(Texture.target() == gli::TARGET_2D, "Only 2d supported now");

	const Renderer::TextureDescriptor desc{path.string(),
										   static_cast<std::size_t>(Texture.extent(0).x),
										   static_cast<std::size_t>(Texture.extent(0).y),
										   Renderer::E_TextureType::TEXTURE_2D,
										   Renderer::E_TextureFormat::D16,
										   true,
										   static_cast<std::uint8_t>(Texture.levels())};

	auto texture = std::make_shared<Textures::C_Texture>(desc);

	Core::C_Application::Get().GetActiveRenderer().GetDevice().AllocateTexture(*texture.get());
	texture->SetParameter(GL_TEXTURE_BASE_LEVEL, 0);
	texture->SetParameter(GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
	glTexParameteriv(Target, GL_TEXTURE_SWIZZLE_RGBA, &Format.Swizzles[0]);

	for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
	{
		glm::tvec3<GLsizei> Extent(Texture.extent(Level));
		if (gli::is_compressed(Texture.format()))
		{
			glCompressedTexSubImage2D(Target, static_cast<GLint>(Level), 0, 0, Extent.x, Extent.y, Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
									  Texture.data(0, 0, Level));
		}
		else
		{
			glTextureSubImage2D(texture->GetTexture(), static_cast<GLint>(Level), 0, 0, Extent.x, Extent.y, Format.External, Format.Type, Texture.data(0, 0, Level));
		}
		texture->SetReadyToUse();
	}


	return texture;
}

} // namespace GLEngine::GLRenderer::Textures