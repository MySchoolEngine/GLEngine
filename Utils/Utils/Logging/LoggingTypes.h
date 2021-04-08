#pragma once

namespace Utils { namespace Logging {

enum class E_Context
{
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

}} // namespace Utils::Logging
