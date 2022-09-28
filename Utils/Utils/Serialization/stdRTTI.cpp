#include <Utils/Serialization/GLMRTTI.h>

#include <filesystem>
#include <rttr/registration>

RTTR_REGISTRATION
{
	rttr::type::register_converter_func([](std::filesystem::path& path, bool& ok) -> std::string {
		ok = true;
		return path.string();
	});
}