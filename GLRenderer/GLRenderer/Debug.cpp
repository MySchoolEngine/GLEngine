#include <GLRendererStdafx.h>

#include <GLRenderer/Debug.h>

#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace GLEngine {
namespace GLRenderer {

const static std::string s_DebugShaderName = "basic-wireframe";

//=================================================================================
const char* glErrorCodeToString(unsigned int code) {
#define codeToStr(c) case c: return #c; break
	switch (code)
	{
		codeToStr(GL_INVALID_ENUM);
		codeToStr(GL_INVALID_VALUE);
		codeToStr(GL_INVALID_OPERATION);
		codeToStr(GL_STACK_OVERFLOW);
		codeToStr(GL_STACK_UNDERFLOW);
		codeToStr(GL_OUT_OF_MEMORY);
	default:
		return "bad value";
		break;
	}

}

//=================================================================================
bool _glErrorCheck(const char* file, const int line)
{
	GLenum status;
	while ((status = glGetError()) != GL_NO_ERROR) {
		std::cout << "[" << file << ":" << line << "] Error (0x"
			<< std::hex << std::setfill('0') << std::setw(4)
			<< status << ":" << glErrorCodeToString(status) << "): "
			// << glewGetErrorString(status)
			<< std::dec
			<< std::endl;
#if _DEBUG
		__debugbreak();
#endif
		return true;
	}
	return false;
}

//=================================================================================
C_DebugDraw & C_DebugDraw::Instance()
{
	static C_DebugDraw    instance; // Guaranteed to be destroyed.
									// Instantiated on first use.
	return instance;
}
#if _DEBUG
//=================================================================================
void C_DebugDraw::SetupAABB()
{
	std::vector<glm::vec4> vertices = {
		{-0.5, -0.5, -0.5, 1.0},
		{0.5, -0.5, -0.5, 1.0},
		{0.5,  0.5, -0.5, 1.0},
		{-0.5,  0.5, -0.5, 1.0},
		{-0.5, -0.5,  0.5, 1.0},
		{0.5, -0.5,  0.5, 1.0},
		{0.5,  0.5,  0.5, 1.0},
		{-0.5,  0.5,  0.5, 1.0},
	};

	std::vector<GLushort> elements = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5, 2, 6, 3, 7
	};

	m_VAOaabb.bind();

	m_VAOaabb.SetBuffer<0, GL_ARRAY_BUFFER>(vertices);
	m_VAOaabb.SetIndexBuffer<1>(elements);
	m_VAOaabb.EnableArray<0>();

	m_VAOaabb.unbind();
}

//=================================================================================
C_DebugDraw::C_DebugDraw()
{
	SetupAABB();
	std::vector<glm::vec4> dummy;
	m_VAOLINE.bind();
	m_VAOLINE.SetBuffer<0, GL_ARRAY_BUFFER>(dummy);
	m_VAOLINE.EnableArray<0>();
	m_VAOLINE.unbind();
}

//=================================================================================
C_DebugDraw::~C_DebugDraw()
{
}

//=================================================================================
void C_DebugDraw::Clear()
{
}

//=================================================================================
void C_DebugDraw::DrawPoint(const glm::vec4 & point, const glm::vec3 & color, const glm::mat4 & modelMatrix)
{
	auto& shdManager = Shaders::C_ShaderManager::Instance();
	auto program = shdManager.GetProgram(s_DebugShaderName);
	shdManager.ActivateShader(program);

	m_VAOLINE.bind();

	glPointSize(5.0f);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), glm::value_ptr(point), GL_DYNAMIC_DRAW);

	program->SetUniform("modelMatrix", modelMatrix);
	program->SetUniform("colorIN", color);

	glDrawArrays(GL_POINTS, 0, 1);

	m_VAOLINE.unbind();
}

//=================================================================================
void C_DebugDraw::DrawPoint(const glm::vec3 & point, const glm::vec3 & color, const glm::mat4 & modelMatrix)
{
	DrawPoint(toVec4(point), color, modelMatrix);
}

//=================================================================================
void C_DebugDraw::DrawAABB(const Physics::Primitives::S_AABB& bbox, const glm::vec3& color /*= glm::vec3(0.0f, 0.0f, 0.0f)*/, const glm::mat4& modelMatrix /*= glm::mat4(1.0f)*/)
{
	auto& shdManager = Shaders::C_ShaderManager::Instance();
	auto program = Shaders::C_ShaderManager::Instance().GetProgram(s_DebugShaderName);
	shdManager.ActivateShader(program);

	glm::vec3 size = bbox.m_Max - bbox.m_Min;
	glm::vec3 center = (bbox.m_Max + bbox.m_Min) / 2.0f;// glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
	glm::mat4 transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);

	/* Apply object's transformation matrix */
	program->SetUniform("modelMatrix", modelMatrix*transform);
	program->SetUniform("colorIN", color);

	m_VAOaabb.bind();
	m_VAOaabb.BindBuffer<1>();

	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));

	m_VAOaabb.unbind();
}

//=================================================================================
void C_DebugDraw::DrawLine(const glm::vec4& pointA, const glm::vec4& pointB, const glm::vec3& color /*= glm::vec3(0.0f, 0.0f, 0.0f)*/)
{
	auto& shdManager = Shaders::C_ShaderManager::Instance();
	auto program = Shaders::C_ShaderManager::Instance().GetProgram(s_DebugShaderName);
	shdManager.ActivateShader(program);

	m_VAOLINE.bind();

	std::vector<glm::vec4> vertices;
	vertices.push_back(pointA);
	vertices.push_back(pointB);

	m_VAOLINE.SetBufferData<0, GL_ARRAY_BUFFER>(vertices, true);

	program->SetUniform("modelMatrix", glm::mat4(1.0f));
	program->SetUniform("colorIN", color);

	glDrawArrays(GL_LINES, 0, 2);

	m_VAOLINE.unbind();
}

//=================================================================================
void C_DebugDraw::DrawLine(const glm::vec3& pointA, const glm::vec3& pointB, const glm::vec3& color /*= glm::vec3(0.0f, 0.0f, 0.0f)*/)
{
	DrawLine(toVec4(pointA), toVec4(pointB), color);
}

//=================================================================================
void C_DebugDraw::DrawLines(const std::vector<glm::vec4>& pairs, const glm::vec3 & color)
{
	auto& shdManager = Shaders::C_ShaderManager::Instance();
	auto program = Shaders::C_ShaderManager::Instance().GetProgram(s_DebugShaderName);
	shdManager.ActivateShader(program);

	m_VAOLINE.bind();

	m_VAOLINE.SetBufferData<0, GL_ARRAY_BUFFER>(pairs, true);

	program->SetUniform("modelMatrix", glm::mat4(1.0f));
	program->SetUniform("colorIN", color);
	
	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(pairs.size()));

	m_VAOLINE.unbind();
}

//=================================================================================
void C_DebugDraw::DrawAxis(const glm::vec4 & origin, const glm::vec4 & up, const glm::vec4 & foreward, glm::mat4 & modelMatrix)
{
	glm::vec4 forewardVec = glm::normalize(foreward - origin);
	glm::vec4 upVec = glm::normalize(up - origin);
	glm::vec4 rightVec = toVec4(glm::normalize(glm::cross(glm::vec3(upVec), glm::vec3(forewardVec))));
	DrawLine(origin, origin + forewardVec, glm::vec3(0.0f, 0.0f, 1.0f));
	DrawLine(origin, origin + upVec, glm::vec3(0.0f, 1.0f, 0.0f));
	DrawLine(origin, origin + rightVec, glm::vec3(1.0f, 0.0f, 0.0f));
}

#endif
}}