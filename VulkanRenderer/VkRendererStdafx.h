#pragma once

#include <Utils/Logging/LoggingMacros.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <exception>
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <map>
#include <numeric>
#include <vector>
#include <random>
#include <regex>
#include <stack>
#include <string>
#include <sstream>
#include <filesystem>

// rendering dependend things
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <imgui.h>

#include <Core/GUID.h>
#include <Core/CoreMacros.h>

#include <VulkanRenderer/VkRendererApi.h>

#include <vulkan/vulkan.h>