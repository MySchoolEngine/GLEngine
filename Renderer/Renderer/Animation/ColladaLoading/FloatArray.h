#pragma once

namespace pugi {
class xml_node;
}

namespace GLEngine::Renderer {

struct S_FloatArray {
	S_FloatArray() = default;
	explicit S_FloatArray(const pugi::xml_node& floatArray);
	S_FloatArray&					   operator=(S_FloatArray&&) = default;
	template <class T> [[nodiscard]] T Get();

	template <class T> [[nodiscard]] std::size_t count() const;

	[[nodiscard]] bool EndOfArray() const;

private:
	std::size_t		  m_Position;
	std::size_t		  m_NumFloats;
	std::stringstream m_FloatsStream;
};

} // namespace GLEngine::Renderer

#include <Renderer/Animation/ColladaLoading/FloatArray.inl>