#pragma once

#include <Core/CoreApi.h>

namespace GLEngine {

// simplest implementation TODO!!!!
using GUID = unsigned int;
static const GUID INVALID_GUID = 0;

CORE_API_EXPORT GUID NextGUID();

}