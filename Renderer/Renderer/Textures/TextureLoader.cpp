#include <RendererStdafx.h>

#include <Renderer/Textures/TextureLoader.h>


//Selects implementation
//Either DevIL or FreeImage
//WARNING - there is a bug in FreeImage implementation! It returns BGRA!
#define USE_DEVIL

#include <IL/il.h>


namespace GLEngine::Renderer::Textures {
//=================================================================================
bool TextureLoader::_isILinitialized = false;

//=================================================================================
bool TextureLoader::loadTexture(const std::filesystem::path& path, MeshData::Texture& t)
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

#if CORE_PLATFORM == CORE_PLATFORM_WIN
	ilLoadImage(path.wstring().c_str());
#else
	ilLoadImage(path.generic_string().c_str());
#endif
	ILenum Error;
	Error = ilGetError();

	if (Error != IL_NO_ERROR)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "DevIL: Failed to load image {}, error: {}", path, Error);
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

}