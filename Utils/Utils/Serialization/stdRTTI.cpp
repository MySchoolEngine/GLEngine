#include <Utils/Serialization/GLMRTTI.h>

#include <filesystem>
#include <rttr/registration>

RTTR_REGISTRATION
{
	rttr::registration::class_<std::filesystem::path>("std::filesystem::path")
      .constructor<>()
      .constructor<const std::filesystem::path&>()
      .constructor<const std::string&>();

	rttr::type::register_converter_func([](std::filesystem::path& path, bool& ok) -> std::string {
		ok = true;
		return path.string();
	});
}