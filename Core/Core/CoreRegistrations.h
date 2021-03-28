#pragma once

#include <Core/Engine.h>

#define REGISTER_GLOBAL_COMPONENT_BUILDER(compName, builderClass)                                                                                                                  \
	const auto GCBReg = []() {                                                                                                                                                     \
		::GLEngine::Core::C_Engine::Instance().RegisterGlobalComponentBuilder(compName, new builderClass());                                                                       \
		return true;                                                                                                                                                               \
	}()
