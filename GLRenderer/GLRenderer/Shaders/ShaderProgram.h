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

namespace Textures {
class C_Texture;
}

namespace Shaders {

class C_ShaderProgram
{
public:
	explicit C_ShaderProgram(GLuint program);
	C_ShaderProgram& operator=(C_ShaderProgram& other) = delete;
	C_ShaderProgram(C_ShaderProgram& rhs) = delete;
	C_ShaderProgram(C_ShaderProgram&& rhs);
	void operator=(C_ShaderProgram&& rhs);
	virtual ~C_ShaderProgram();

	bool IsActive() const { return m_bIsActive; }

	void BindUBO(std::shared_ptr<Buffers::C_UniformBuffer>) const;

#if _DEBUG
	void SetName(const std::string& name) noexcept;
#else
	inline void SetName(const std::string& name) noexcept {}
#endif

	// replace this
	inline GLuint GetProgram() const { return m_Program; }

	//=================================================================================
	// Uniforms
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
	template<class N> void SetUniform(N name, const std::vector<glm::mat4>& value);

	template<class T> int FindLocation(T name);

	//=================================================================================
	// UBOs
	template<class T> int FindUniformBlockLocation(T name) const;


	//=================================================================================
	// Textures
	void BindSampler(const Textures::C_Texture& texture, unsigned int unit);
	void BindSampler(const Textures::C_Texture& texture, const std::string& samplerName);

#if _DEBUG
	void SetPaths(std::vector<std::filesystem::path>&& paths);
	bool IsExpired() const;
#else
	bool IsExpired() const { return false; }
#endif

private:
#if _DEBUG
	std::string m_name;
	std::filesystem::file_time_type m_LastUpdate;
	std::vector<std::filesystem::path> m_Paths;

	std::filesystem::file_time_type GetLastUpdate() const;
#endif
	GLuint m_Program;
	std::map<std::size_t, GLint> m_uniformMap;
	bool m_bIsActive : 1;


	void useProgram();
	void disableProgram();
	void DestroyProgram();

	friend class C_ShaderManager;
};

}}}
#include <GLRenderer/Shaders/ShaderProgram.inl>