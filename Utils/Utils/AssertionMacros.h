#pragma once

#define GL_ASSERT(cond, message, ...) \
if(!(cond)) { \
	::Utils::Logging::C_LoggingSystem::Instance().Log(::Utils::Logging::E_Level::Error, ::Utils::Logging::E_Context::MODULE_CTX, __LINE__, __FILENAME__, message, __VA_ARGS__); \
	__debugbreak(); \
}

