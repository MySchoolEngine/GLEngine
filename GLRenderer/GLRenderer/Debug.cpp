#include <GLRendererStdafx.h>

#include <GLRenderer/Debug.h>

#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <Renderer/Animation/Skeleton.h>

#include <Physics/Primitives/Frustum.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace GLEngine {
namespace GLRenderer {

const static std::string s_DebugShaderName = "basic-wireframe";
const static std::string s_MergedShaderName = "MergedWireframes";

#define codeToStr(c) case c: return #c; break

//=================================================================================
const char* glErrorCodeToString(unsigned int code) {
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
const char* glErrorTypeToString(GLenum code) {
	switch (code)
	{
		codeToStr(GL_DEBUG_TYPE_ERROR);
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
#ifdef GL_ENGINE_DEBUG
		__debugbreak();
#endif
		return true;
	}
	return false;
}

//=================================================================================
void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
	if (type != GL_DEBUG_TYPE_ERROR) return;
	CORE_LOG(E_Level::Error, E_Context::Render,"GL CALLBACK: {} type = {}, severity = 0x{:x}, message = {}",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		glErrorTypeToString(type), severity, message);
#ifdef GL_ENGINE_DEBUG
	__debugbreak();
#endif
}

//=================================================================================
C_DebugDraw & C_DebugDraw::Instance()
{
	static C_DebugDraw    instance; // Guaranteed to be destroyed.
									// Instantiated on first use.
	return instance;
}

#ifdef GL_ENGINE_DEBUG
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

	m_VAOaabb.NameArray("AABB");
	m_VAOaabb.NameBuffer<0>("vertices");
}

//=================================================================================
C_DebugDraw::C_DebugDraw()
{
	SetupAABB();
	std::vector<glm::vec4> dummy4;
	std::vector<glm::vec3> dummy3;

	m_VAOlines.bind();
	m_VAOlines.SetBuffer<0, GL_ARRAY_BUFFER>(dummy4);
	m_VAOlines.SetBuffer<1, GL_ARRAY_BUFFER>(dummy3);
	m_VAOlines.EnableArray<0>();
	m_VAOlines.EnableArray<1>();
	m_VAOlines.unbind();
}

//=================================================================================
C_DebugDraw::~C_DebugDraw() = default;

//=================================================================================
void C_DebugDraw::Clear()
{
}

//=================================================================================
void C_DebugDraw::DrawPoint(const glm::vec4 & point, const glm::vec3 & color, const glm::mat4 & modelMatrix)
{
	m_PointsVertices.push_back(modelMatrix * point);
	m_PointsColors.push_back(color);
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
	m_LinesVertices.push_back(pointA);
	m_LinesVertices.push_back(pointB);
	// we need two copies as we have two vertices
	m_LinesColors.push_back(color);
	m_LinesColors.push_back(color);
}

//=================================================================================
void C_DebugDraw::DrawLine(const glm::vec3& pointA, const glm::vec3& pointB, const glm::vec3& color /*= glm::vec3(0.0f, 0.0f, 0.0f)*/)
{
	DrawLine(toVec4(pointA), toVec4(pointB), color);
}

//=================================================================================
void C_DebugDraw::DrawLines(const std::vector<glm::vec4>& pairs, const glm::vec3 & color)
{
	m_LinesVertices.insert(m_LinesVertices.end(), pairs.begin(), pairs.end());
	m_LinesColors.insert(m_LinesColors.end(), pairs.size(), color);
}

//=================================================================================
void C_DebugDraw::DrawBone(const glm::vec3& position, const Renderer::Animation::S_Joint& joint)
{
	const auto locTransformation	= glm::inverse((joint.m_InverseBindTransfomr));
	const glm::vec4 modelDest		= (locTransformation * glm::vec4(0.f, 0.f, 0.0f, 1.f));
	const glm::vec3 dest			= glm::vec3(modelDest / modelDest.w);
	const glm::vec3 boneOffset		= dest - position;

	const float bumpFactor = .33f;
	const auto BumpPosition = position + boneOffset * bumpFactor;

	glm::vec3 tangent;

	auto c1 = glm::cross(boneOffset, glm::vec3(0.0, 0.0, 1.0));
	auto c2 = glm::cross(boneOffset, glm::vec3(0.0, 1.0, 0.0));

	if (glm::length(c1) > glm::length(c2))
	{
		tangent = c1;
	}
	else
	{
		tangent = c2;
	}

	tangent=glm::normalize(tangent);
	const auto bitangent = glm::cross(tangent, boneOffset);

	const float bumpSize = .05f;
	const glm::vec3 Offset1 = tangent * bumpSize;
	const glm::vec3 Offset2 = bitangent * bumpSize;


	DrawLine(position, BumpPosition, glm::vec3(1, 1, 1));
	DrawLine(BumpPosition, dest, glm::vec3(0, 1, 0));

	// square around the bump
	DrawLine(BumpPosition + Offset1 + Offset2, BumpPosition + Offset1 - Offset2);
	DrawLine(BumpPosition + Offset1 + Offset2, BumpPosition - Offset1 + Offset2);
	DrawLine(BumpPosition - Offset1 + Offset2, BumpPosition - Offset1 - Offset2);
	DrawLine(BumpPosition + Offset1 - Offset2, BumpPosition - Offset1 - Offset2);

	// pos to bump
	DrawLine(position, BumpPosition + Offset1 + Offset2);
	DrawLine(position, BumpPosition + Offset1 - Offset2);
	DrawLine(position, BumpPosition - Offset1 - Offset2);
	DrawLine(position, BumpPosition - Offset1 + Offset2);

	// bump to dest
	DrawLine(dest, BumpPosition + Offset1 + Offset2);
	DrawLine(dest, BumpPosition + Offset1 - Offset2);
	DrawLine(dest, BumpPosition - Offset1 - Offset2);
	DrawLine(dest, BumpPosition - Offset1 + Offset2);
	
	for (const auto& child : joint.m_Children)
	{
		DrawBone(dest, child);
	}
}

//=================================================================================
void C_DebugDraw::DrawSkeleton(const glm::vec3& root, const Renderer::Animation::C_Skeleton& skeleton)
{
	const auto locTransformation = glm::inverse((skeleton.m_Root->m_InverseBindTransfomr));
	const glm::vec4 modelDest = (locTransformation * glm::vec4(0.f, 0.f, 0.0f, 1.f));
	const glm::vec3 dest = glm::vec3(modelDest / modelDest.w);
	const glm::vec3 boneOffset = dest - root;

	for (const auto& child : skeleton.m_Root->m_Children)
	{
		DrawBone(root + boneOffset, child);
	}
}

//=================================================================================
void C_DebugDraw::DrawAxis(const glm::vec3& origin, const glm::vec3& up, const glm::vec3& foreward, glm::mat4 & modelMatrix)
{
	glm::vec3 forewardVec = (foreward - origin);
	glm::vec3 upVec = glm::normalize(up - origin);
	glm::vec3 rightVec = toVec4(glm::normalize(glm::cross(glm::vec3(up), glm::vec3(foreward))));
	const auto originInModelSpace = modelMatrix * glm::vec4(origin, 1.0f);
	DrawLine(originInModelSpace, modelMatrix * glm::vec4((origin + foreward), 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	DrawLine(originInModelSpace, modelMatrix * glm::vec4((origin + up), 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	DrawLine(originInModelSpace, modelMatrix * glm::vec4((origin + rightVec), 1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}

//=================================================================================
void C_DebugDraw::DrawGrid(const glm::vec4& origin, unsigned short linesToSide, glm::mat4& modelMatrix /*= glm::mat4(1.0f)*/)
{
	int limit = linesToSide;
	// cross for center
	DrawLine(origin + glm::vec4(-limit, 0, 0, 1.f), origin + glm::vec4(limit, 0, 0, 1.f));
	DrawLine(origin + glm::vec4(0, 0, -limit, 1.f), origin + glm::vec4(0, 0, limit, 1.f));

	for (int i = 1; i <= limit; ++i) {
		// lines in positive direction from origin
		DrawLine(origin + glm::vec4(-limit, 0, i, 1.f), origin + glm::vec4(limit, 0, i, 1.f));
		DrawLine(origin + glm::vec4(i, 0, -limit, 1.f), origin + glm::vec4(i, 0, limit, 1.f));

		// lines in negative direction from origin
		DrawLine(origin + glm::vec4(-limit, 0, -i, 1.f), origin + glm::vec4(limit, 0, -i, 1.f));
		DrawLine(origin + glm::vec4(-i, 0, -limit, 1.f), origin + glm::vec4(-i, 0, limit, 1.f));
	}
}

//=================================================================================
void C_DebugDraw::DrawFrustum(const Physics::Primitives::C_Frustum& frust, const glm::vec3& color)
{
	const auto& position = frust.GetPosition();
	const auto& upVector = frust.GetUpVector();
	const auto& forward	 = frust.GetForeward();
	const auto  fnear	 = frust.GetNear();
	const auto  ffar	 = frust.GetFar();
	const auto  fov      = frust.GetFov();
	const auto	aspect	 = frust.GetAspect();

	const glm::vec3 nearCenter = glm::vec3(position + (forward * fnear));
	const glm::vec3 farCenter = glm::vec3(position + (forward * ffar));

	const glm::vec3 left = glm::cross(upVector, forward);

	const float tanHalfHFOV = tanf(glm::radians(fov / 2.0f));
	const float tanHalfVFOV = tanf(glm::radians((fov * aspect) / 2.0f));

	const float xn = fnear * tanHalfVFOV;
	const float xf = ffar * tanHalfVFOV;
	const float yn = fnear * tanHalfHFOV;
	const float yf = ffar * tanHalfHFOV;

	const auto nlt = glm::vec4(nearCenter + (xn * left) + upVector * yn, 1.0f);
	const auto nrt = glm::vec4(nearCenter - (xn * left) + upVector * yn, 1.0f);
	const auto nlb = glm::vec4(nearCenter + (xn * left) - upVector * yn, 1.0f);
	const auto nrb = glm::vec4(nearCenter - (xn * left) - upVector * yn, 1.0f);
	const auto flt = glm::vec4(farCenter + (xf * left) + upVector * yf, 1.0f);
	const auto frt = glm::vec4(farCenter - (xf * left) + upVector * yf, 1.0f);
	const auto flb = glm::vec4(farCenter + (xf * left) - upVector * yf, 1.0f);
	const auto frb = glm::vec4(farCenter - (xf * left) - upVector * yf, 1.0f);

	std::vector<glm::vec4> lines;
	lines.push_back(nlt); lines.push_back(nrt);
	lines.push_back(nlb); lines.push_back(nrb);
	lines.push_back(nlt); lines.push_back(nlb);
	lines.push_back(nrt); lines.push_back(nrb);

	lines.push_back(flt); lines.push_back(frt);
	lines.push_back(flb); lines.push_back(frb);
	lines.push_back(flt); lines.push_back(flb);
	lines.push_back(frt); lines.push_back(frb);

	lines.push_back(nlt); lines.push_back(flt);
	lines.push_back(nrt); lines.push_back(frt);
	lines.push_back(nlb); lines.push_back(flb);
	lines.push_back(nrb); lines.push_back(frb);

	DrawLines(lines, color);
	DrawPoint(position);
	DrawLine(position, position + forward, glm::vec3(0, 1, 0));
	DrawLine(position, position + upVector, glm::vec3(1, 0, 0));
}

//=================================================================================
void C_DebugDraw::DrawMergedGeoms()
{
	auto& shdManager = Shaders::C_ShaderManager::Instance();
	auto program = Shaders::C_ShaderManager::Instance().GetProgram(s_MergedShaderName);
	shdManager.ActivateShader(program);
	m_VAOlines.bind();
	std::vector<glm::vec4> mergedVertices(m_LinesVertices);
	std::vector<glm::vec3> mergedColors(m_LinesColors);
	mergedVertices.insert(mergedVertices.end(), m_PointsVertices.begin(), m_PointsVertices.end());
	mergedColors.insert(mergedColors.end(), m_PointsColors.begin(), m_PointsColors.end());
	m_VAOlines.SetBufferData<0, GL_ARRAY_BUFFER>(mergedVertices, true);
	m_VAOlines.SetBufferData<1, GL_ARRAY_BUFFER>(mergedColors, true);

	const auto lineVertices = static_cast<GLsizei>(m_LinesVertices.size());
	const auto pointsVertices = static_cast<GLsizei>(m_PointsVertices.size());
	if (lineVertices > 0) {
		glDrawArrays(GL_LINES, 0, lineVertices);
	}
	if (pointsVertices) {
		glPointSize(5.0f);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glDrawArrays(GL_POINTS, lineVertices, pointsVertices);
	}


	m_VAOlines.unbind();
	shdManager.DeactivateShader();

	m_LinesVertices.clear();
	m_LinesColors.clear();

	m_PointsVertices.clear();
	m_PointsColors.clear();
}

#endif
}}