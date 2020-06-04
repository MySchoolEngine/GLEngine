#include <GLRendererStdafx.h>

#include <GLRenderer/Textures/TextureLoader.h>

#include <GLRenderer/Textures/Texture.h>


//Selects implementation
//Either DevIL or FreeImage
//WARNING - there is a bug in FreeImage implementation! It returns BGRA!
#define USE_DEVIL

#include <IL/il.h>
#include <gli/gli.hpp>


namespace GLEngine::GLRenderer::Textures {
//=================================================================================
bool TextureLoader::_isILinitialized = false;

//=================================================================================
bool TextureLoader::loadTexture(const std::filesystem::path& path, Renderer::MeshData::Texture& t)
{
	if (!_isILinitialized)
	{
		ilInit();
		_isILinitialized = true;
	}

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);

	ilLoadImage(path.wstring().c_str());

	ILenum Error;
	Error = ilGetError();

	if (Error != IL_NO_ERROR)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "DevIL: Failed to load image {}, error: {}", path.generic_string(), Error);
		t.height = t.width = 1;
		t.m_name = "Error";
		t.data = std::shared_ptr<unsigned char>(new unsigned char[4 * t.width * t.height]);
		t.data.get()[0] = 255;
		t.data.get()[1] = 0;
		t.data.get()[2] = 0;
		t.data.get()[3] = 0;
		return false;
	}

	//Convert the texture to R8G8B8A8
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	t.m_name = path.generic_string();
	t.width = ilGetInteger(IL_IMAGE_WIDTH);
	t.height = ilGetInteger(IL_IMAGE_HEIGHT);

	t.data = std::shared_ptr<unsigned char>(new unsigned char[4 * t.width*t.height]);
	memcpy(t.data.get(), ilGetData(), 4 * t.width*t.height);

	ilDeleteImage(image);

	return true;
}

//=================================================================================
std::shared_ptr<GLEngine::GLRenderer::Textures::C_Texture> TextureLoader::LoadAndInitTexture(const std::filesystem::path& path)
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