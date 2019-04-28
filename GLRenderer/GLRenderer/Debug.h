/**
 * ==============================================
 * @file 		Debug.h
 * @date 		2018/03/17 19:30
 * @project 	Computer Graphics Project
 * @faculty 	Faculty of Information Technology
 * @university 	Brno University of Technology
 * 
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 * ==============================================
 * @brief		In this file you can find debug functions for both OpenGL 
 *				and C++.
 * ==============================================
 */
 
#pragma once

#include <GLRenderer/MeshLoading/Scene.h>

#include <GLRenderer/VAO/VAO.h>

namespace GLEngine {
namespace GLRenderer {

/** ==============================================
 * @method:    _glErrorCheck
 * @return:    bool true if error occurs
 * @param: 	   const std::string file
 * @param: 	   const int line
 * @brief	   You should not use this directly, use ErrorCheck() macro instead
 ** ==============================================*/
bool _glErrorCheck(const char* file, const int line);

/** ==============================================
* @method:    ErrorCheck
* @return:    bool true if error occurs
* @brief	  Checks for OpenGL API errors.
*
* @note When _DEBUG is true this will debug break your program when error occurs.
*		You can use that for debug purposes. When error occurs this will print
*		Basic error info and name of file and line where error occurs.
*
* @todo Create define for disabling this macro completely.
* @warning Make sure this is not called after OpenGL context destruction, otherwise
* this will produce false positive warnings
** ==============================================*/
#ifndef SPEEDPROFILE
#define ErrorCheck() _glErrorCheck(__FILE__, __LINE__)
#else
#define ErrorCheck()
#endif


/** ==============================================
* @method:    DestructorFullCheck
* @brief	  Use that in function where you want to destruct everything from class representing
*			  OpenGL resource.
*
* @todo Create define for disabling this macro completely.
* @warning Make sure this is not called after OpenGL context destruction, otherwise
* this will produce false positive warnings
** ==============================================*/
#if _DEBUG
#define DestructorFullCheck() _glErrorCheck(__FILE__, __LINE__); std::cout<<__FUNCTION__<<std::endl
#else
#define DestructorFullCheck()
#endif

//=================================================================================
// Forward declarations
namespace GLW {
class C_ShaderProgram;
}

#if _DEBUG
//=================================================================================
/**
 * ==============================================
 * @class C_DebugDraw
 *
 * @brief	Helpers for OpenGL visual debug.
 *
 * @note	When _DEBUG macro is set to 0 defined this class is defined
 *			with empty methods implementations so you can feel free
 *			to use it around application without #if _DEBUG
 *
 * @author 	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * @date 	2018/03/17
 * ==============================================
 */
class C_DebugDraw {
public:
	//Singleton stuff
	C_DebugDraw(C_DebugDraw const&) = delete;
	void operator=(C_DebugDraw const&) = delete;
	static C_DebugDraw& Instance();
	~C_DebugDraw();

	void Clear();


	void DrawPoint(const glm::vec3& point, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f), const glm::mat4& modelMatrix = glm::mat4(1.0f));
	void DrawPoint(const glm::vec4& point, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f), const glm::mat4& modelMatrix = glm::mat4(1.0f));
	void DrawAABB(const Physics::Primitives::S_AABB& bbox, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f), const glm::mat4& modelMatrix = glm::mat4(1.0f));
	void DrawLine(const glm::vec4& pointA, const glm::vec4& pointB, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f));
	void DrawLine(const glm::vec3& pointA, const glm::vec3& pointB, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f));
	void DrawLines(const std::vector<glm::vec4>& pairs, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f));

	void DrawAxis(const glm::vec4& origin, const glm::vec4& up, const glm::vec4& foreward, glm::mat4& modelMatrix = glm::mat4(1.0f));
private:
	/**
	 * ==============================================
	 * @method:		toVec4
	 * @fullName:	C_DebugDraw::toVec4
	 * @return:		glm::vec4
	 * @param: 		const glm::vec3 & vec
	 * @brief		Helper to transform vec3 to vec4
	 * @todo		Check on difference between inline function and macro, should be same
	 *				because of compilers optimizations
	 * ==============================================
	 */
	inline glm::vec4 toVec4(const glm::vec3& vec) const noexcept { return glm::vec4(vec, 1.0f); };
	/**
	 * ==============================================
	 * @method:    SetupAABB
	 * @fullName:  C_DebugDraw::SetupAABB
	 * @return:    void
	 * @brief	   Fills VAO, VBO and IBO for drawing AABB debug
	 * ==============================================
	 */
	void SetupAABB();

	C_DebugDraw();

	GLuint m_VAOline;
	GLuint m_VBOline;
	VAO::C_GLVAO<2> m_VAOAABBs;
};
#else
//=================================================================================
class C_DebugDraw {
public:
	//Singleton stuff
	C_DebugDraw(C_DebugDraw const&) = delete;
	void operator=(C_DebugDraw const&) = delete;
	static C_DebugDraw& Instance();
	~C_DebugDraw() {};

	void Clear() {};

	void DrawPoint(const glm::vec3& point, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f), const glm::mat4& modelMatrix = glm::mat4(1.0f)) {};
	void DrawPoint(const glm::vec4& point, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f), const glm::mat4& modelMatrix = glm::mat4(1.0f)) {};
	void DrawAABB(const Physics::Primitives::S_AABB& bbox, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f), const glm::mat4& modelMatrix = glm::mat4(1.0f)) {};
	void DrawLine(const glm::vec4& pointA, const glm::vec4& pointB, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f)) {};
	void DrawLine(const glm::vec3& pointA, const glm::vec3& pointB, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f)) {};
	void DrawLines(const std::vector<glm::vec4>& pairs, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f)) {};

	void DrawAxis(const glm::vec4& origin, const glm::vec4& up, const glm::vec4& foreward, glm::mat4& modelMatrix = glm::mat4(1.0f)) {};
private:

	C_DebugDraw() {};
};
#endif
}}