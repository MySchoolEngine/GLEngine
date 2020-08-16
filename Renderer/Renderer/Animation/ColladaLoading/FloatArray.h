#pragma once

namespace pugi
{
class xml_node;
}

namespace GLEngine::Renderer::Animation {

struct S_FloatArray
{
	S_FloatArray() = default;
	explicit S_FloatArray(const pugi::xml_node& floatArray);
	S_FloatArray& operator=(S_FloatArray&&) = default;
	template<class T>
	[[nodiscard]] T Get() = delete;
	template<>
	[[nodiscard]] float Get();
	template<>
	[[nodiscard]] glm::vec2 Get();
	template<>
	[[nodiscard]] glm::vec3 Get();
	template<>
	[[nodiscard]] glm::vec4 Get();
	template<>
	[[nodiscard]] glm::mat4 Get();

	template<class T>
	[[nodiscard]] std::size_t count() const = delete;
	template<>
	[[nodiscard]] std::size_t count<float>() const { return m_NumFloats; }
	template<>
	[[nodiscard]] std::size_t count<glm::vec2>() const { return m_NumFloats/2; }
	template<>
	[[nodiscard]] std::size_t count<glm::vec3>() const { return m_NumFloats/3; }
	template<>
	[[nodiscard]] std::size_t count<glm::vec4>() const { return m_NumFloats/4; }
	template<>
	[[nodiscard]] std::size_t count<glm::mat4>() const { return m_NumFloats/16; }

	[[nodiscard]] bool EndOfArray() const;
private:
	std::size_t					m_Position;
	std::size_t					m_NumFloats;
	std::stringstream		m_FloatsStream;
};

}

#include <Renderer/Animation/ColladaLoading/FloatArray.inl>