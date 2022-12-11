#pragma once

namespace pugi {
class xml_node;
}

namespace GLEngine::Renderer {
struct C_VCount {
public:
	explicit C_VCount(const pugi::xml_node& vcount);

	[[nodiscard]] unsigned int Get();

	[[nodiscard]] bool EndOfArray() const;

private:
	std::stringstream m_IntStream;
};
} // namespace GLEngine::Renderer