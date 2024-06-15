#include <glm/glm.hpp>

#include <optional>
#include <rttr/registration>
#include <sstream>
#include <format>

bool SerializeString(const glm::vec2& vec, std::string& ret)
{
	ret = "(" + std::format("{}", vec.x) + "," + std::format("{}", vec.y) + ")";
	return true;
}

bool SerializeString(const glm::vec3& vec, std::string& ret)
{
	ret = "(" + std::format("{}", vec.x) + "," + std::format("{}", vec.y) + "," + std::format("{}", vec.z) + ")";
	return true;
}

bool SerializeString(const glm::vec4& vec, std::string& ret)
{
	ret = "(" + std::format("{}", vec.x) + "," + std::format("{}", vec.y) + "," + std::format("{}", vec.z) + "," + std::format("{}", vec.w) + ")";
	return true;
}

bool SerializeString(const glm::mat4& mat, std::string& ret)
{
	ret = "(";
	for (int i = 0; i < 4; ++i)
	{
		std::string v;
		SerializeString(mat[i], v);
		ret += v;
	}

	ret += ")";
	return true;
}

bool SerializeString(const std::vector<glm::vec3>& vec, std::string& ret)
{
	for (const auto& v : vec)
	{
		std::string v1;
		SerializeString(v, v1);
		ret += v1;
	}
	return true;
}

template <int numElements> bool DeserializeFloatList(std::stringstream& ss, glm::vec<numElements, float, glm::defaultp>& vec)
{
	std::string value;

	for (int i = 0; i < numElements; ++i)
	{
		if (std::getline(ss, value, ','))
		{
			vec[i] = std::stof(value);
		}
		else
			return false;
	}
	return true;
}

bool DeserializeString(const std::string& str, glm::vec2& vec)
{
	if (str[0] != '(' || str[str.length() - 1] != ')')
		return false;
	std::stringstream ss(str.substr(1, str.length() - 2));

	return DeserializeFloatList(ss, vec);
}

bool DeserializeString(const std::string& str, glm::vec3& vec)
{
	if (str[0] != '(' || str[str.length() - 1] != ')')
		return false;
	std::stringstream ss(str.substr(1, str.length() - 2));

	return DeserializeFloatList(ss, vec);
}

bool DeserializeString(const std::string& str, glm::vec4& vec)
{
	if (str[0] != '(' || str[str.length() - 1] != ')')
		return false;
	std::stringstream ss(str.substr(1, str.length() - 2));

	return DeserializeFloatList(ss, vec);
}

bool DeserializeString(const std::string& str, glm::mat4& mat)
{
	if (str[0] != '(' || str[str.length() - 1] != ')')
		return false;
	std::stringstream ss(str.substr(1, str.length() - 2));

	std::size_t pos		 = 1;
	std::size_t endOfVec = 0;
	for (int i = 0; i < 4; ++i)
	{
		endOfVec = str.find_first_of(')', pos);
		if(!DeserializeString(str.substr(pos, endOfVec - pos + 1), mat[i]))
			return false;
		pos = endOfVec + 1;
	}
	return true;
}

bool DeserializeString(const std::string& str, std::vector<glm::vec3>& ret)
{
	if (str[0] != '(' || str[str.length() - 1] != ')')
		return false; // wrong format

	std::size_t pos		 = 0;
	std::size_t endOfVec = 0;
	while (endOfVec = str.find_first_of(')', pos), endOfVec != std::string::npos)
	{
		std::stringstream ss(str.substr(pos + 1, endOfVec - 1 - pos));
		glm::vec3		  vec;
		if (!DeserializeFloatList(ss, vec))
			return false;
		ret.push_back(vec);
		pos = endOfVec + 1;
	}
	return true;
}

#include <Utils/Serialization/SerializationUtils.h>


// clang-format off
RTTR_REGISTRATION
{
	rttr::registration::class_<glm::vec2>("vec2")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("x", &glm::vec2::x)
		.property("y", &glm::vec2::y);
	rttr::registration::class_<glm::vec3>("vec3")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("x", &glm::vec3::x)
		.property("y", &glm::vec3::y)
		.property("z", &glm::vec3::z);
	rttr::registration::class_<glm::vec4>("vec4")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("x", &glm::vec4::x)
		.property("y", &glm::vec4::y)
		.property("z", &glm::vec4::z)
		.property("w", &glm::vec4::w);
	rttr::registration::class_<glm::mat4>("mat4")
	 	.constructor<>()(rttr::policy::ctor::as_object);

	REGISTER_SERIALIZATION(glm::vec2);
	REGISTER_SERIALIZATION(glm::vec3);
	REGISTER_SERIALIZATION(glm::vec4);
	REGISTER_SERIALIZATION(glm::mat4);
	REGISTER_SERIALIZATION(std::vector<glm::vec3>);
}
// clang-format on