#include <RendererStdafx.h>

#include <Renderer/RayCasting/Frame.h>

namespace GLEngine::Renderer {
//=================================================================================
S_Frame::S_Frame(const glm::vec3 &normal) : X(1, 0, 0), Y(0, 1, 0), Z(0, 0, 1) {
  SetFromNormal(normal);
}

//=================================================================================
void S_Frame::SetFromNormal(const glm::vec3 &normal) {
  const glm::vec3 tmpY = Y = glm::normalize(normal);
  const glm::vec3 tmpX =
      (std::abs(tmpY.x) > 0.99f) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
  Z = glm::normalize(glm::cross(tmpX, tmpY));
  X = glm::cross(Z, tmpY);
}

} // namespace GLEngine::Renderer