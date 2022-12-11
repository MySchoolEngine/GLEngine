#include <RendererStdafx.h>

#include <Renderer/Animation/ColladaLoading/VCount.h>

#include <pugixml.hpp>

namespace GLEngine::Renderer {

//=================================================================================
C_VCount::C_VCount(const pugi::xml_node& vcount)
{
	std::string_view ints = vcount.child_value();
	ints.remove_suffix(ints.size() - ints.find_last_not_of(" \n\t\r\n\0") - 1);
	m_IntStream << ints;
}

//=================================================================================
unsigned int C_VCount::Get()
{
	unsigned int ret;
	GLE_ASSERT(EndOfArray() == false, "You are reading behind the streams end.");
	m_IntStream >> ret;
	GLE_ASSERT(m_IntStream.bad() == false, "read was successful"); // read was successful
	return ret;
}

//=================================================================================
bool C_VCount::EndOfArray() const
{
	return m_IntStream.rdbuf()->in_avail() == 0 || m_IntStream.eof();
}

} // namespace GLEngine::Renderer