#include <Core/CoreMacros.h>

#include <Utils/SSE/Vec3.h>

GL_PUSH_WARNINGS()
#pragma warning(disable : 4996)
#ifdef __GNUC__
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <fmt/format.h>
#include <rttr/registration.h>
GL_POP_WARNINGS()


#include <optional>
#include <sstream>

bool SerializeString(const ::Utils::SSE::Vec3& vec, std::string& ret)
{
	const auto vecGLM = static_cast<glm::vec3>(vec);
	ret = "(" + fmt::format("{}", vecGLM.x) + "," + fmt::format("{}", vecGLM.y) + "," + fmt::format("{}", vecGLM.z) + ")";
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


bool DeserializeString(const std::string& str, ::Utils::SSE::Vec3& sseVec)
{
	if (str[0] != '(' || str[str.length() - 1] != ')')
		return false;
	std::stringstream ss(str.substr(1, str.length() - 2));
	glm::vec3		  vec;
	if (!DeserializeFloatList(ss, vec))
	{
		return false;
	}
	sseVec = ::Utils::SSE::Vec3(vec);
	return true;
}

#include <Utils/Serialization/SerializationUtils.h>


// clang-format off
RTTR_REGISTRATION
{
	rttr::registration::class_<::Utils::SSE::Vec3>("::Utils::SSE::Vec3")
		.constructor<>()(rttr::policy::ctor::as_object);
	REGISTER_SERIALIZATION(::Utils::SSE::Vec3);
}
// clang-format on