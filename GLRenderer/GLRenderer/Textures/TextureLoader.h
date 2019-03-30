#pragma once

//Selects implementation
//Either DevIL or FreeImage
//WARNING - there is a bug in FreeImage implementation! It returns BGRA!
#define USE_DEVIL

#include <iostream>
#include <cstdlib>

#include <IL/il.h>

#include "Scene.hpp"


class TextureLoader
{
public:
    bool loadTexture(const char* path, Texture& t);

private:
    static bool _isILinitialized;
};