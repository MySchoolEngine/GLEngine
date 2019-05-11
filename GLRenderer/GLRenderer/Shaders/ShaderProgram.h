/**
 * ==============================================
 * \file 		ShaderProgram.h
 * \date 		2018/03/17 19:07
 * \project 	Computer Graphics Project
 * \faculty 	Faculty of Information Technology 
 * \university 	Brno University of Technology
 *
 * \author Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * ==============================================
 */

#pragma once

#include <Renderer/IResource.h>

#include <glm/gtc/type_ptr.hpp>

namespace GLEngine {
namespace GLRenderer {

namespace Buffers {
class C_UniformBuffer;
}

namespace Shaders {

class C_ShaderProgram
{
public:
	C_ShaderProgram(GLuint program);
	C_ShaderProgram& operator=(C_ShaderProgram& other) = delete;
	C_ShaderProgram(C_ShaderProgram& rhs) = delete;
	virtual ~C_ShaderProgram();

	bool IsActive() const { return m_bIsActive; }

	void BindUBO(std::shared_ptr<Buffers::C_UniformBuffer>) const;

#if _DEBUG
	inline void SetName(const std::string& name) noexcept { m_name = name; }
#else
	inline void SetName(const std::string& name) noexcept {}
#endif

	// replace this
	inline GLuint GetProgram() const { return m_Program; }

	template<class N, class T> void SetUniform(N name, T value);
	template<class N> void SetUniform(N name, const bool& value);
	template<class N> void SetUniform(N name, const int & value);
	template<class N> void SetUniform(N name, const float & value);
	template<class N> void SetUniform(N name, const glm::mat4 & value);
	template<class N> void SetUniform(N name, const glm::vec4 & value);
	template<class N> void SetUniform(N name, const glm::vec3 & value);
	template<class N> void SetUniform(N name, const glm::ivec2 & value);
	template<class N> void SetUniform(N name, const glm::vec2 & value);
	template<class N> void SetUniform(N name, const std::vector<int> & value);
	template<class N> void SetUniform(N name, const std::vector<float> & value);

	template<class T> int FindLocation(T name);
	template<> int FindLocation(const char* name);
	template<> int FindLocation(const std::string& name);

	template<class T> int FindUniformBlockLocation(T name) const;
	template<> int FindUniformBlockLocation(const char* name) const;
	template<> int FindUniformBlockLocation(const std::string& name) const;

private:
#if _DEBUG
	std::string m_name;
#endif
	GLuint m_Program;
	std::map<std::size_t, GLint> m_uniformMap;
	bool m_bIsActive : 1;


	void useProgram();
	void disableProgram();

	friend class C_ShaderManager;
};
}
}
}
#include <GLRenderer/Shaders/ShaderProgram.inl>