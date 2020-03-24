/** ==============================================
 * @file 		FrameConstantsBuffer.h
 * @date 		2018/05/16 20:29
 * @project 	Opengl Shadow Rendering
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
 
#pragma once

#include <GLRenderer\Buffers\UniformBuffer.h>

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace GLEngine {
namespace GLRenderer {
namespace Buffers {
namespace UBO {

/** ==============================================
 * @class C_FrameConstantsBuffer
 *
 * @brief	Define shader values constant through whole
 *			frame.
 *
 * @author 	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * @date 	2018/05/16
 ** ==============================================*/
class C_FrameConstantsBuffer : public C_UniformBuffer {
public:
	C_FrameConstantsBuffer(const std::string& blockName, unsigned int index);

	virtual void UploadData() const override;

	const glm::vec4& GetCameraPosition() const { return m_CameraPosition; }
	void SetCameraPosition(const glm::vec4& val) { m_CameraPosition = val; }
	const glm::mat4& GetView() const { return m_ViewMat; }
	void SetView(const glm::mat4& val) { m_ViewMat = val; }
	const glm::mat4& GetProjection() const { return m_ProjectionMat; }
	void SetProjection(const glm::mat4& val) { m_ProjectionMat = val; }
	const glm::vec3& GetSunPosition() const { return m_SunPosition; }
	void SetSunPosition(const glm::vec3& val) { m_SunPosition = val; }

	float GetAmbientStrength() const { return m_AmbientStrength; }
	void SetAmbientStrength(float strength) { m_AmbientStrength = strength; }
private:
	glm::vec4 m_CameraPosition;
	glm::mat4 m_ViewMat;
	glm::mat4 m_ProjectionMat;
	glm::vec3 m_SunPosition;
	float			m_AmbientStrength;
};
}}}}