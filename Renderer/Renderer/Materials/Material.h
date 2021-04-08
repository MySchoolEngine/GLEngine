#pragma once

#include <Renderer/Mesh/Scene.h>

#include <Utils/RefCounter.h>

#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {
class RENDERER_API_EXPORT C_Material
    : Utils::RefCounted<Utils::SingleThreadCounter> {
public:
  C_Material(const std::string &name);
  C_Material(const MeshData::Material &material);
  C_Material(const C_Material &other) = delete;
  C_Material &operator=(const C_Material &other) = delete;
  C_Material(C_Material &&other);

  C_Material clone() const;
  bool IsChanged() const;
  void CleanChangeFlag();

  void SetDiffuseColor(const glm::vec3 &color);
  void SetRoughness(float roughness);

  void SetNormalMap(void *texture);
  void SetRoughnessMap(void *texture);
  void SetColorMap(void *texture);
  void *GetColorMap() const { return m_ColorMap; }
  void *GetNormalMap() const { return m_NormalMap; }

  const std::string &GetName() const { return m_Name; }
  const glm::vec3 &GetColor() const { return m_Color; }
  float GetRoughness() const { return m_Roughness; }

  int GetMaterialIndex() const;
  void SetMaterialIndex(int index) { m_MaterialIndex = index; }

  void SetShininess(float value) { m_Shininess = value; }
  float GetShininess() const { return m_Shininess; }

private:
  std::string m_Name;
  glm::vec3 m_Color;
  float m_Roughness;
  void *m_ColorMap;
  void *m_NormalMap;
  void *m_RoughnessMap;
  bool m_Changed : 1;
  int m_MaterialIndex;
  float m_Shininess;
};
} // namespace GLEngine::Renderer