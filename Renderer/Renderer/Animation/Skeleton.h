#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Animation/Joint.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace GLEngine::Renderer::Animation {

class RENDERER_API_EXPORT C_Skeleton {
public:
  C_Skeleton();
  [[nodiscard]] std::size_t GetNumBones() const;
  void SetNumBonse(std::size_t num);
  void ApplyPoseToBones(std::vector<glm::mat4> &poseData) const;

public:
  std::unique_ptr<S_Joint> m_Root;
  std::size_t m_NumBones;
};

} // namespace GLEngine::Renderer::Animation