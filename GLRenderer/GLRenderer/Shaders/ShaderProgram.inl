#pragma once

#include <glm/gtc/type_ptr.hpp>

namespace GLEngine::GLRenderer::Shaders {

//=================================================================================
template <> inline int C_ShaderProgram::FindLocation(const char* name)
{
	GLint location = -1;
	auto  hash	   = std::hash<std::string>{}(name);

	const auto it = m_UniformMap.find(hash);
	if (it != m_UniformMap.end())
	{
		location = it->second;
	}
	else
	{
		location		   = glGetUniformLocation(m_Program, name);
		m_UniformMap[hash] = location;
	}

#if _DEBUG
	if (location < 0)
	{
		CORE_LOG(E_Level::Warning, E_Context::Render, "Program '{}' doesn't have uniform: {}", m_name, name);
	}
#endif

	return location;
}

//=================================================================================
template <> inline int C_ShaderProgram::FindLocation(const std::string& name)
{
	GLint location = -1;
	auto  hash	   = std::hash<std::string>{}(name);

	const auto it = m_UniformMap.find(hash);
	if (it != m_UniformMap.end())
	{
		location = it->second;
	}
	else
	{
		location		   = glGetUniformLocation(m_Program, name.c_str());
		m_UniformMap[hash] = location;
	}

#if _DEBUG
	if (location < 0)
	{
		CORE_LOG(E_Level::Warning, E_Context::Render, "Program '{}' doesn't have uniform: {}", m_name, name);
	}
#endif

	return location;
}

//=================================================================================
template <class N> void C_ShaderProgram::SetUniform(N name, const int& value)
{
	auto loc = FindLocation(name);
	if (loc >= 0)
	{
		glUniform1i(loc, value);
	}
}

//=================================================================================
template <class N> void C_ShaderProgram::SetUniform(N name, const bool& value)
{
	auto loc = FindLocation(name);
	if (loc >= 0)
	{
		glUniform1i(loc, value);
	}
}

//=================================================================================
template <class N> void C_ShaderProgram::SetUniform(N name, const glm::mat4& value)
{
	auto loc = FindLocation(name);
	if (loc >= 0)
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
	}
}

//=================================================================================
template <class N> void C_ShaderProgram::SetUniform(N name, const glm::vec4& value)
{
	auto loc = FindLocation(name);
	if (loc >= 0)
	{
		glUniform4fv(loc, 1, glm::value_ptr(value));
	}
}

//=================================================================================
template <class N> void C_ShaderProgram::SetUniform(N name, const glm::vec3& value)
{
	auto loc = FindLocation(name);
	if (loc >= 0)
	{
		glUniform3fv(loc, 1, glm::value_ptr(value));
	}
}

//=================================================================================
template <class N> void C_ShaderProgram::SetUniform(N name, const glm::ivec2& value)
{
	auto loc = FindLocation(name);
	if (loc >= 0)
	{
		glUniform2iv(loc, 1, glm::value_ptr(value));
	}
}

//=================================================================================
template <class N> void C_ShaderProgram::SetUniform(N name, const glm::vec2& value)
{
	auto loc = FindLocation(name);
	if (loc >= 0)
	{
		glUniform2fv(loc, 1, glm::value_ptr(value));
	}
}

//=================================================================================
template <class N> void C_ShaderProgram::SetUniform(N name, const std::vector<float>& value)
{
	auto loc = FindLocation(name);
	if (loc >= 0)
	{
		glUniform1fv(loc, static_cast<GLsizei>(value.size()), (GLfloat*)(value.data()));
	}
}

//=================================================================================
template <class N> void C_ShaderProgram::SetUniform(N name, const std::vector<int>& value)
{
	auto loc = FindLocation(name);
	if (loc >= 0)
	{
		glUniform1iv(loc, static_cast<GLsizei>(value.size()), (value.data()));
	}
}

//=================================================================================
template <class N> void C_ShaderProgram::SetUniform(N name, const float& value)
{
	auto loc = FindLocation(name);
	if (loc >= 0)
	{
		glUniform1f(loc, static_cast<GLfloat>(value));
	}
}

//=================================================================================
template <class N> void C_ShaderProgram::SetUniform(N name, const std::vector<glm::mat4>& value)
{
	auto loc = FindLocation(name);
	if (loc >= 0)
	{
		glUniformMatrix4fv(loc, static_cast<GLsizei>(value.size()), GL_FALSE, (GLfloat*)(value.data()));
	}
}

//=================================================================================
template <> inline int C_ShaderProgram::FindUniformBlockLocation<const std::string&>(const std::string& name) const
{
	return glGetUniformBlockIndex(m_Program, name.c_str());
}

//=================================================================================
template <> inline int C_ShaderProgram::FindUniformBlockLocation<const char*>(const char* name) const
{
	return glGetUniformBlockIndex(m_Program, name);
}

} // namespace GLEngine::GLRenderer::Shaders