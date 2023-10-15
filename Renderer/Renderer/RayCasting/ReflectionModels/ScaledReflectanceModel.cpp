#include <RendererStdafx.h>

#include <Renderer/RayCasting/ReflectionModels/ScaledReflectanceModel.h>

namespace GLEngine::Renderer {

//=================================================================================
C_ScaledReflectanceModel::C_ScaledReflectanceModel(I_ReflectionModel* model, float scale)
	: m_Model(model)
	, m_Scale(scale)
{
}

//=================================================================================
Colours::T_Colour C_ScaledReflectanceModel::f(const glm::vec3& wi, const glm::vec3& wo) const
{
	return m_Scale * m_Model->f(wi, wo);
}

//=================================================================================
Colours::T_Colour C_ScaledReflectanceModel::SampleF(const glm::vec3& wi, glm::vec3& wo, const S_Frame& frame, const glm::vec2& rng, float* pdf) const
{
	return m_Scale * m_Model->SampleF(wi, wo, frame, rng, pdf);
}

//=================================================================================
float C_ScaledReflectanceModel::Pdf(const glm::vec3& wi, const glm::vec3& wo) const
{
	return m_Model->Pdf(wi, wo);
}

}
