#include <filesystem>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <rttr/registration.h>
#pragma warning(pop)


bool SerializeString(const std::vector<float>& vec, std::string& ret)
{
	ret = std::string("(") + ")";
	return true;
}

bool DeserializeString(const std::string& str, std::vector<float>& vec)
{
	return false;
}

#include <Utils/Serialization/SerializationUtils.h>

RTTR_REGISTRATION
{
	rttr::registration::class_<std::filesystem::path>("std::filesystem::path").constructor<>().constructor<const std::filesystem::path&>().constructor<const std::string&>();

	rttr::type::register_converter_func([](std::filesystem::path& path, bool& ok) -> std::string {
		ok = true;
		return path.string();
	});

	REGISTER_SERIALIZATION(std::vector<float>);
}