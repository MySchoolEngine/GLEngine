#include <Utils/Logging/LoggingTypes.h>

#include <rttr/registration.h>

RTTR_REGISTRATION
{
	rttr::registration::enumeration<Utils::Logging::E_Context>("E_Context")(
		rttr::value("Core",			Utils::Logging::E_Context::Core), 
		rttr::value("Render",		Utils::Logging::E_Context::Render),
		rttr::value("Entity",		Utils::Logging::E_Context::Entity)
	);

	rttr::registration::enumeration<Utils::Logging::E_Level>("E_Level")(
		rttr::value("Error",		Utils::Logging::E_Level::Error), 
		rttr::value("Warning",		Utils::Logging::E_Level::Warning),
		rttr::value("Info",			Utils::Logging::E_Level::Info),
		rttr::value("Debug",		Utils::Logging::E_Level::Debug)
	);
}
