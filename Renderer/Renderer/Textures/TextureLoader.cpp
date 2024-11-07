#include <RendererStdafx.h>

#include <Renderer/Mesh/Scene.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>
#include <Renderer/Textures/TextureLoader.h>

#include <Utils/ScopeFinalizer.h>

// Selects implementation
// Either DevIL or FreeImage
// WARNING - there is a bug in FreeImage implementation! It returns BGRA!
#define USE_DEVIL

#include <IL/il.h>

namespace GLEngine::Renderer::Textures {
//=================================================================================
bool	   TextureLoader::_isILinitialized = false;
std::mutex TextureLoader::m_Mutex;

//=================================================================================
bool TextureLoader::loadTexture(const std::filesystem::path& path, MeshData::Texture& t)
{
	Init();

	auto					image = ilLoadTexture(path);
	Utils::C_ScopeFinalizer finalizer([image]() { ilDeleteImage(image); });
	const ILenum			Error = ilGetError();

	if (Error != IL_NO_ERROR)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "DevIL: Failed to load image {}, error: {}", path, Error);
		t.height = t.width = 1;
		t.m_name		   = "Error";
		t.data			   = std::shared_ptr<unsigned char>(new unsigned char[4 * t.width * t.height]);
		t.data.get()[0]	   = 255;
		t.data.get()[1]	   = 0;
		t.data.get()[2]	   = 0;
		t.data.get()[3]	   = 0;
		return false;
	}

	// Convert the texture to R8G8B8A8
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	t.m_name = path.generic_string();
	t.width	 = ilGetInteger(IL_IMAGE_WIDTH);
	t.height = ilGetInteger(IL_IMAGE_HEIGHT);

	t.data = std::shared_ptr<unsigned char>(new unsigned char[4 * t.width * t.height]);
	memcpy(t.data.get(), ilGetData(), 4 * t.width * t.height);

	return true;
}

//=================================================================================
std::unique_ptr<I_TextureViewStorage> TextureLoader::loadTexture(const std::filesystem::path& path)
{
	// memory leak, ilDeleteImage(image); could be skipped
	Init();

	auto					image = ilLoadTexture(path);
	Utils::C_ScopeFinalizer finalizer([image]() { ilDeleteImage(image); });
	const ILenum			Error = ilGetError();

	if (Error != IL_NO_ERROR)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "DevIL: Failed to load image {}, error: {}", path, Error);
		return nullptr;
	}

	const auto							  width			= ilGetInteger(IL_IMAGE_WIDTH);
	const auto							  height		= ilGetInteger(IL_IMAGE_HEIGHT);
	std::unique_ptr<I_TextureViewStorage> textureBuffer = nullptr;

	if (ilGetInteger(IL_PALETTE_TYPE) != IL_PAL_NONE)
	{
		ilConvertImage(ilGetInteger(IL_PALETTE_BASE_TYPE), IL_FLOAT);
	}
	if (ilGetInteger(IL_IMAGE_TYPE) != IL_FLOAT)
		ilConvertImage(ilGetInteger(IL_IMAGE_FORMAT), IL_FLOAT);

	if (ilGetInteger(IL_IMAGE_BPC) == 1)
	{
		textureBuffer = std::make_unique<C_TextureViewStorageCPU<std::uint8_t>>(width, height, static_cast<std::uint8_t>(ilGetInteger(IL_IMAGE_CHANNELS)));
	}
	else if (ilGetInteger(IL_IMAGE_BPC) == 4)
	{
		textureBuffer = std::make_unique<C_TextureViewStorageCPU<float>>(width, height, static_cast<std::uint8_t>(ilGetInteger(IL_IMAGE_CHANNELS)));
	}
	else
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Image have unsupported bytes per channel {}", ilGetInteger(IL_IMAGE_BPC));
		return nullptr;
	}

	switch (ilGetInteger(IL_IMAGE_FORMAT))
	{
	case IL_RGB: {
		textureBuffer->SetChannels({E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::None});
		break;
	}
	case IL_RGBA: {
		textureBuffer->SetChannels({E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::Alpha});
		break;
	}
	case IL_BGR: {
		textureBuffer->SetChannels({E_TextureChannel::Blue, E_TextureChannel::Green, E_TextureChannel::Red, E_TextureChannel::None});
		break;
	}
	case IL_BGRA: {
		textureBuffer->SetChannels({E_TextureChannel::Blue, E_TextureChannel::Green, E_TextureChannel::Red, E_TextureChannel::Alpha});
		break;
	}
	default: {
		CORE_LOG(E_Level::Error, E_Context::Render, "Image have unsupported format {}", ilGetInteger(IL_IMAGE_FORMAT));
		break;
	}
	}

	textureBuffer->SetData(ilGetData(), static_cast<std::size_t>(width) * height);

	return textureBuffer;
}

//=================================================================================
void TextureLoader::Init()
{
	if (!_isILinitialized)
	{
		ilInit();
		_isILinitialized = true;
	}
}

//=================================================================================
unsigned int TextureLoader::ilLoadTexture(const std::filesystem::path& path)
{
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

	return image;
}

//=================================================================================
bool TextureLoader::SaveTexture(const std::filesystem::path& path, I_TextureViewStorage* view)
{
	Init();

	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);

	ilTexImage(view->GetDimensions().x, view->GetDimensions().y, 1, view->GetNumElements(), IL_RGB, IL_FLOAT, view->GetData());
	Utils::C_ScopeFinalizer finalizer([image]() { ilDeleteImage(image); });

	bool result;
#if CORE_PLATFORM == CORE_PLATFORM_WIN
	result = ilSaveImage(path.wstring().c_str());
#else
	result = ilSaveImage(path.generic_string().c_str());
#endif
	return result;
}

} // namespace GLEngine::Renderer::Textures