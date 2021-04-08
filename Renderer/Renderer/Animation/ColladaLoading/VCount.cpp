#include <RendererStdafx.h>

#include <Renderer/Animation/ColladaLoading/VCount.h>

#include <pugixml.hpp>

namespace GLEngine::Renderer::Animation {

//=================================================================================
C_VCount::C_VCount(const pugi::xml_node &vcount) {
  const std::string_view ints = vcount.child_value();
  m_IntStream << ints;
}

//=================================================================================
int C_VCount::Get() {
  int ret;
  m_IntStream >> ret;
  return ret;
}

//=================================================================================
bool C_VCount::EndOfArray() const {
  return m_IntStream.rdbuf()->in_avail() == 0;
}

} // namespace GLEngine::Renderer::Animation