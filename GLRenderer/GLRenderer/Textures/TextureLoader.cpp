#include "TextureLoader.hpp"

bool TextureLoader::_isILinitialized = false;

bool TextureLoader::loadTexture(const char* path, Texture& t)
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
#pragma warning(push)  
#pragma warning(disable : 4996) // I trust Ing. Kobrtek's code
    wchar_t* wpath = new wchar_t[strlen(path)+1];
    std::mbstowcs(wpath, path, strlen(path));
    wpath[strlen(path)] = 0;
    ilLoadImage(wpath);
	delete[] wpath;
#pragma warning(pop) // but not mine code!

    ILenum Error;
    Error = ilGetError();

    if (Error != IL_NO_ERROR)
    {
        std::wcerr << "DevIL: Failed to load image " << path << ", error: " << Error << std::endl;
        return false;
    }

    //Convert the texture to R8G8B8A8
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	t.m_name = path;
    t.width = ilGetInteger(IL_IMAGE_WIDTH);
    t.height = ilGetInteger(IL_IMAGE_HEIGHT);

    t.data = std::shared_ptr<unsigned char>(new unsigned char[4 * t.width*t.height]);
    memcpy(t.data.get(), ilGetData(), 4 * t.width*t.height);

    ilDeleteImage(image);

    return true;
}