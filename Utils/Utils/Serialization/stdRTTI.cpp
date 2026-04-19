#include <Core/CoreMacros.h>

#include <filesystem>
#include <ranges>

GL_PUSH_WARNINGS()
#pragma warning(disable : 4996)
#ifdef __GNUC__
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <rttr/registration.h>
GL_POP_WARNINGS()

template <class T> bool SerializeString(const std::vector<T>& vec, std::string& ret)
{
	std::stringstream ss(ret);
	std::for_each(std::begin(vec), std::end(vec), [&ss, sep = ""](T x) mutable {
		ss << sep << x;
		sep = ",";
	});
	ret = ss.str();
	return true;
}

template <class T> bool SerializeString(std::vector<T>* vec, std::string& ret)
{
	std::stringstream ss(ret);
	std::for_each(std::begin(*vec), std::end(*vec), [&ss, sep = ""](T x) mutable {
		ss << sep << x;
		sep = ",";
	});
	ret = ss.str();
	return true;
}

template <class T> bool DeserializeString(const std::string& str, std::vector<T>& vec)
{
	std::stringstream ss(str);
	while (ss.good())
	{
		T val;
		ss >> val;
		ss.get(); // removes delimiter
		vec.push_back(val);
	}
	return true;
}

template <class T> bool DeserializeString(const std::string& str, std::vector<T>* vec)
{
	std::stringstream ss(str);
	while (ss.good())
	{
		T val;
		ss >> val;
		ss.get(); // removes delimiter
		vec->push_back(val);
	}
	return true;
}

#include <Utils/Serialization/SerializationUtils.h>

RTTR_REGISTRATION
{
	rttr::registration::class_<std::filesystem::path>("std::filesystem::path").constructor<>().constructor<const std::filesystem::path&>().constructor<const std::string&>();

	rttr::type::register_converter_func([](std::filesystem::path& path, bool& ok) -> std::string {
		ok = true;
		return path.string();
	});

	REGISTER_SERIALIZATION(std::vector<unsigned int>);
	REGISTER_SERIALIZATION(std::vector<int>);
	REGISTER_SERIALIZATION(std::vector<float>);
	REGISTER_SERIALIZATION(std::vector<double>);

	REGISTER_SERIALIZATION_PTR(std::vector<unsigned int>);
	REGISTER_SERIALIZATION_PTR(std::vector<int>);
	REGISTER_SERIALIZATION_PTR(std::vector<float>);
	REGISTER_SERIALIZATION_PTR(std::vector<double>);
}