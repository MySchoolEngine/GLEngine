#pragma once

#include <Utils/Allocation/StackAllocator.h>

namespace GLEngine::AI {
using T_BTNodeID = unsigned int;
using T_BTNodeInstanceID = unsigned int;

constexpr static T_BTNodeID s_InvalidNodeID = std::numeric_limits<T_BTNodeID>::max();
}