#pragma once 

namespace GLEngine {
namespace GLRenderer {
namespace Shaders {

//=================================================================================
template<>
inline int C_ShaderProgram::FindLocation(const char* name)
{
	GLint location = -1;
	auto hash = std::hash<std::string>{}(name);

	const auto it = m_uniformMap.find(hash);
	if (it != m_uniformMap.end()) {
		location = it->second;
	}
	else {
		location = glGetUniformLocation(m_Program, name);
		m_uniformMap[hash] = location;
	}

	if (location < 0) {
		CORE_LOG(E_Level::Warning, E_Context::Render, "Program '{}' doesn't have uniform: {}", m_name, name);
	}

	return location;
}

//=================================================================================
template<>
inline int C_ShaderProgram::FindLocation(const std::string& name)
{
	GLint location = -1;
	auto hash = std::hash<std::string>{}(name);

	const auto it = m_uniformMap.find(hash);
	if (it != m_uniformMap.end()) {
		location = it->second;
	}
	else {
		location = glGetUniformLocation(m_Program, name.c_str());
		m_uniformMap[hash] = location;
	}

	if (location < 0) {
		CORE_LOG(E_Level::Warning, E_Context::Render, "Program '{}' doesn't have uniform: {}", m_name, name);
	}

	return location;
}

//=================================================================================
template<class N>
void C_ShaderProgram::SetUniform(N name, const int & value)
{
	glUniform1i(FindLocation(name), value);
}

//=================================================================================
template<class N>
void C_ShaderProgram::SetUniform(N name, const bool& value)
{
	glUniform1i(FindLocation(name), value);
}

//=================================================================================
template<class N>
void C_ShaderProgram::SetUniform(N name, const glm::mat4 & value)
{
	glUniformMatrix4fv(FindLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

//=================================================================================
template<class N>
void C_ShaderProgram::SetUniform(N name, const glm::vec4 & value)
{
	glUniform4fv(FindLocation(name), 1, glm::value_ptr(value));
}

//=================================================================================
template<class N>
void C_ShaderProgram::SetUniform(N name, const glm::vec3 & value)
{
	glUniform3fv(FindLocation(name), 1, glm::value_ptr(value));
}

//=================================================================================
template<class N>
void C_ShaderProgram::SetUniform(N name, const glm::ivec2 & value)
{
	glUniform2iv(FindLocation(name), 1, glm::value_ptr(value));
}

//=================================================================================
template<class N>
void C_ShaderProgram::SetUniform(N name, const glm::vec2 & value)
{
	glUniform2fv(FindLocation(name), 1, glm::value_ptr(value));
}

//=================================================================================
template<class N>
void C_ShaderProgram::SetUniform(N name, const std::vector<float> & value)
{
	glUniform1fv(FindLocation(name), static_cast<GLsizei>(value.size()), (GLfloat*)(value.data()));
}

//=================================================================================
template<class N>
void C_ShaderProgram::SetUniform(N name, const std::vector<int> & value)
{
	glUniform1iv(FindLocation(name), static_cast<GLsizei>(value.size()), (value.data()));
}

//=================================================================================
template<class N>
void C_ShaderProgram::SetUniform(N name, const float & value)
{
	glUniform1f(FindLocation(name), static_cast<GLfloat>(value));
}

//=================================================================================
template<class N>
void C_ShaderProgram::SetUniform(N name, const std::vector<glm::mat4>& value)
{
	glUniformMatrix4fv(FindLocation(name), static_cast<GLsizei>(value.size()), GL_FALSE, (GLfloat*)(value.data()));
}

//=================================================================================
template<>
int C_ShaderProgram::FindUniformBlockLocation(const std::string& name) const
{
	return glGetUniformBlockIndex(m_Program, name.c_str());
}

//=================================================================================
template<>
int C_ShaderProgram::FindUniformBlockLocation(const char* name) const
{
	return glGetUniformBlockIndex(m_Program, name);
}

}}}