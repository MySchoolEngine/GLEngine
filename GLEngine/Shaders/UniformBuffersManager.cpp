#include "UniformBuffersManager.h"

#include "GLW/Buffers/UniformBuffer.h"
#include "GLW/ShaderProgram.h"
#include "Debug/Debug.h"

#include <algorithm>


//=================================================================================
C_UniformBuffersManager::C_UniformBuffersManager()
{

}

//=================================================================================
C_UniformBuffersManager& C_UniformBuffersManager::Instance()
{
	static C_UniformBuffersManager    instance; // Guaranteed to be destroyed.
										// Instantiated on first use.
	return instance;
}

//=================================================================================
void C_UniformBuffersManager::PrintStatistics() const
{

}

//=================================================================================
void C_UniformBuffersManager::Clear()
{
	m_UBOs.clear();
	DestructorFullCheck();
}

//=================================================================================
void C_UniformBuffersManager::BindUBOs(GLW::C_ShaderProgram* program)
{
	assert(program);
	for (auto& ubo : m_UBOs) {
		if (ubo->IsActive()) {
			program->BindUBO(ubo);
		}
	}
}

//=================================================================================
C_UniformBuffersManager::T_UBOSmartPtr C_UniformBuffersManager::GetBufferByName(const std::string& name) const
{
	return *std::find_if(m_UBOs.begin(), m_UBOs.end(), [name](const T_UBOSmartPtr& ubo) {
		return name == ubo->GetBlockName();
	});
}

//=================================================================================
void C_UniformBuffersManager::ProcessUBOBindingPoints(std::shared_ptr<GLW::C_ShaderProgram> program)
{
	int i = 0;
	for (const auto & ubo : m_UBOs)
	{
		if (!ubo) {
			continue;
		}
		auto uniformBlockIndex = program->FindUniformBlockLocation<const std::string&>(ubo->GetBlockName());
		if (uniformBlockIndex != GL_INVALID_INDEX) {
			glUniformBlockBinding(program->GetProgram(), uniformBlockIndex, i);
		}
		++i;
	}
}
