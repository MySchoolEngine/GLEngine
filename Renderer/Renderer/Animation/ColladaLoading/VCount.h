#pragma once

namespace pugi
{
class xml_node;
}

namespace GLEngine::Renderer::Animation {
struct C_VCount
{
public:
	C_VCount(pugi::xml_node& vcount);

	int Get();

	bool EndOfArray() const;
private:

	std::stringstream		m_IntStream;
};
}