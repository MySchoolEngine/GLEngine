#pragma once

#include <Core/CoreMacros.h>

namespace Core {
namespace Logging {


enum class API_EXPORT E_Context {
	Core,
	Render,
};

enum class API_EXPORT E_Level {
	Error,
	Warning,
	Info,
	Debug,
};

}
}
