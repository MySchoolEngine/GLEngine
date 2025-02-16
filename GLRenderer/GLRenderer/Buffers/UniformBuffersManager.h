/** ==============================================
 * @file 		UniformBuffersManager.h
 * @date 		2018/03/17 20:04
 * @project 	Computer Graphics Project
 * @faculty 	Faculty of Information Technology
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/

#pragma once


namespace GLEngine::GLRenderer {

namespace Shaders {
class C_ShaderProgram;
}

namespace Buffers {

class C_UniformBuffer;

/** ==============================================
 * @class C_UniformBuffersManager
 *
 * @brief	Manages UBO's over application.
 *
 * @note	This class is intended to store and bind UBO's to
 *			shader programs. Also, possibly prints some statistics
 *
 * @todo	It would be nice to extend statistics to memory usage details
 *
 * @author 	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * @date 	2018/03/17
 ** ==============================================*/
class C_UniformBuffersManager {
public:
	using T_UBOSmartPtr = std::shared_ptr<C_UniformBuffer>;

	// Singleton stuff
	C_UniformBuffersManager(C_UniformBuffersManager const&) = delete;
	void							operator=(C_UniformBuffersManager const&) = delete;
	static C_UniformBuffersManager& Instance();

	void PrintStatistics() const;
	void Clear();

	void													  BindUBOs(const Shaders::C_ShaderProgram* program) const;
	template <class T, typename... Params> std::shared_ptr<T> CreateUniformBuffer(const std::string& name, Params&&... params);
	// should be used only in debug
	T_UBOSmartPtr GetBufferByName(const std::string& name) const;


	void ProcessUBOBindingPoints(std::shared_ptr<Shaders::C_ShaderProgram> program) const;

	// get number of bytes used by uniform buffers
	[[nodiscard]] std::size_t GetUsedMemory() const { return m_UsedMemory; }

private:
	C_UniformBuffersManager();
	std::vector<T_UBOSmartPtr> m_BindingPoint;
	int						   m_MaxBindingPoints;
	std::size_t				   m_UsedMemory;
};

//=================================================================================
template <class T, typename... Params> std::shared_ptr<T> C_UniformBuffersManager::CreateUniformBuffer(const std::string& name, Params&&... params)
{
	GLE_ASSERT(m_BindingPoint.size() < m_MaxBindingPoints, "Too many uniform buffers");
	auto ubo = std::make_shared<T>(name, static_cast<unsigned int>(m_BindingPoint.size()), std::forward<Params>(params)...);
	GLE_ASSERT(ubo, "Unable to allocate UBO {}", name);
	m_BindingPoint.push_back(ubo);
	m_UsedMemory += ubo->GetBufferSize();

	return ubo;
}
} // namespace Buffers
} // namespace GLEngine::GLRenderer