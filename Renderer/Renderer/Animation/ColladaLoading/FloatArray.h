#pragma once

namespace pugi
{
class xml_node;
}

namespace GLEngine::Renderer::Animation {

struct S_FloatArray
{
	S_FloatArray(const pugi::xml_node& floatArray);
	template<class T>
	T Get() = delete;
	template<>
	float Get();
	template<>
	glm::vec2 Get();
	template<>
	glm::vec3 Get();
	template<>
	glm::vec4 Get();
	template<>
	glm::mat4 Get();

	template<class T>
	std::size_t count() const = delete;
	template<>
	std::size_t count<float>() const { return m_NumFloats; }
	template<>
	std::size_t count<glm::vec2>() const { return m_NumFloats/2; }
	template<>
	std::size_t count<glm::vec3>() const { return m_NumFloats/3; }
	template<>
	std::size_t count<glm::vec4>() const { return m_NumFloats/4; }
	template<>
	std::size_t count<glm::mat4>() const { return m_NumFloats/16; }

	bool EndOfArray() const;
private:
	std::size_t					m_Position;
	std::size_t					m_NumFloats;
	std::stringstream		m_FloatsStream;
};

}

#include <Renderer/Animation/ColladaLoading/FloatArray.inl>