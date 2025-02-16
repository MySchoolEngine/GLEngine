/** ==============================================
 * @file 		FrameConstantsBuffer.h
 * @date 		2018/05/16 20:29
 * @project 	Opengl Shadow Rendering
 * @faculty 	Faculty of Information Technology
 * @university 	Brno University of Technology
 *
 *
 * @brief	Define shader values constant through whole
 *			frame.
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 * @date 		2018/05/16
 ** ==============================================*/

#pragma once

#include <GLRenderer/Buffers/UniformBuffer.h>

namespace GLEngine::GLRenderer::Buffers::UBO {
class C_FrameConstantsBuffer : public C_UniformBuffer {
public:
	C_FrameConstantsBuffer(const std::string& blockName, unsigned int index);

	void UploadData() const override;

	[[nodiscard]] std::size_t GetBufferSize() const override;

	[[nodiscard]] const glm::vec4& GetCameraPosition() const { return m_CameraPosition; }
	void						   SetCameraPosition(const glm::vec4& val) { m_CameraPosition = val; }
	[[nodiscard]] const glm::mat4& GetView() const { return m_ViewMat; }
	void						   SetView(const glm::mat4& val) { m_ViewMat = val; }
	[[nodiscard]] const glm::mat4& GetProjection() const { return m_ProjectionMat; }
	void						   SetProjection(const glm::mat4& val) { m_ProjectionMat = val; }

	[[nodiscard]] float GetAmbientStrength() const { return m_AmbientStrength; }
	void				SetAmbientStrength(float strength) { m_AmbientStrength = strength; }

	void SetFrameTime(float time) { m_Time = time; }
	void SetNearPlane(float distance) { m_NearPlane = distance; }
	void SetFarPlane(float distance) { m_FarPlane = distance; }

private:
	glm::vec4 m_CameraPosition;
	glm::mat4 m_ViewMat;
	glm::mat4 m_ProjectionMat;
	float	  m_AmbientStrength;
	float	  m_Time;
	float	  m_NearPlane;
	float	  m_FarPlane;
};
} // namespace GLEngine::GLRenderer::Buffers::UBO