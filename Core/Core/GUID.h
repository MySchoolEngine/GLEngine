#pragma once

#include <Core/CoreMacros.h>

namespace GLEngine {

// simplest implementation TODO!!!!
using GUID = unsigned int;
static const GUID INVALID_GUID = 0;

API_EXPORT GUID NextGUID();

}