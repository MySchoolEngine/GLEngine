#pragma once

#include <Utils/Logging/LoggingMacros.h>

#include <Utils/STLAfx.h>

// rendering dependent things
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

// debug utils
#include <GLRenderer/RenderDoc/RenderDocTools.h>
#include <GLRenderer/Debug.h>

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>
#include <GLRenderer/Helpers/GLSLTypeHelpers.h>

#include <Core/GUID.h>
#include <Core/CoreMacros.h>

#include <GLRenderer/GLRendererApi.h>

#include <Renderer/Definitions.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <rttr/registration.h>
#pragma warning(pop)