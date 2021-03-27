#pragma once

namespace Utils::Logging {

enum class E_Context
{
	Audio,
	Core,
	Render,
	Entity,
};

enum class E_Level
{
	Error,
	Warning,
	Info,
	Debug,
};

} // namespace Utils::Logging
