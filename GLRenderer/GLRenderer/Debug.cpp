#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/GLEnable.h>
#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Debug.h>
#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureUnitManager.h>

#include <Renderer/Animation/IPose.h>
#include <Renderer/Animation/Skeleton.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Mesh/Geometry.h>

#include <Core/Application.h>

#include <Utils/DebugBreak.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

namespace GLEngine::GLRenderer {

const static std::string s_DebugShaderName		= "basic-wireframe";
const static std::string s_MergedShaderName		= "MergedWireframes";
const static std::string s_OctahedronShaderName = "OctahedronMapping";

#define codeToStr(c)                                                                                                                                                               \
	case c:                                                                                                                                                                        \
		return #c;                                                                                                                                                                 \
		break

//=================================================================================
const char* glErrorCodeToString(unsigned int code)
{
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
	}
}

//=================================================================================
const char* glErrorTypeToString(GLenum code)
{
	switch (code)
	{
		codeToStr(GL_DEBUG_TYPE_ERROR);
	default:
		return "bad value";
	}
}

//=================================================================================
bool _glErrorCheck(const char* file, const int line)
{
	GLenum status;
	while ((status = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "[" << file << ":" << line << "] Error (0x" << std::hex << std::setfill('0') << std::setw(4) << status << ":" << glErrorCodeToString(status)
				  << "): "
				  // << glewGetErrorString(status)
				  << std::dec << std::endl;
		GL_DebugBreak();
		return true;
	}
	return false;
}

//=================================================================================
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;
	if (type != GL_DEBUG_TYPE_ERROR)
		return;
	CORE_LOG(E_Level::Error, E_Context::Render, "GL CALLBACK: {} type = {}, severity = 0x{:x}, message = {}", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			 glErrorTypeToString(type), severity, message);
	GL_DebugBreak();
}

//=================================================================================
C_DebugDraw& C_DebugDraw::Instance()
{
	static C_DebugDraw instance; // Guaranteed to be destroyed.
								 // Instantiated on first use.
	return instance;
}

#ifdef GL_ENGINE_DEBUG
//=================================================================================
void C_DebugDraw::SetupAABB()
{
	std::vector<glm::vec4> vertices = {
		{-0.5, -0.5, -0.5, 1.0}, {0.5, -0.5, -0.5, 1.0}, {0.5, 0.5, -0.5, 1.0}, {-0.5, 0.5, -0.5, 1.0},
		{-0.5, -0.5, 0.5, 1.0},	 {0.5, -0.5, 0.5, 1.0},	 {0.5, 0.5, 0.5, 1.0},	{-0.5, 0.5, 0.5, 1.0},
	};

	std::vector<GLushort> elements = {0, 1, 2, 3, 4, 5, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7};

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
	: m_OctahedronMesh(nullptr)
{
	SetupAABB();
	std::vector<glm::vec3> dummy4;
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
void C_DebugDraw::DrawPoint(const glm::vec3& point, const Colours::T_Colour& color, const glm::mat4& modelMatrix)
{
	m_PointsVertices.emplace_back(modelMatrix * toVec4(point));
	m_PointsColors.push_back(color);
}

//=================================================================================
void C_DebugDraw::DrawAABB(const Physics::Primitives::S_AABB& bbox, const Colours::T_Colour& color, const glm::mat4& modelMatrix)
{
	glm::vec3 size		= bbox.m_Max - bbox.m_Min;
	glm::vec3 center	= (bbox.m_Max + bbox.m_Min) / 2.0f; // glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
	glm::mat4 transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);

	m_AABBTransform.emplace_back(modelMatrix * transform);
	m_AABBColor.emplace_back(color);
}

//=================================================================================
void C_DebugDraw::DrawLine(const glm::vec3& pointA, const glm::vec3& pointB, const Colours::T_Colour& color)
{
	DrawLine(pointA, pointB, color, color);
}
//=================================================================================
void C_DebugDraw::DrawLine(const glm::vec3& pointA, const glm::vec3& pointB, const Colours::T_Colour& colorA, const Colours::T_Colour& colorB)
{
	m_LinesVertices.emplace_back(toVec4(pointA));
	m_LinesVertices.emplace_back(toVec4(pointB));
	// we need two copies as we have two vertices
	m_LinesColors.push_back(colorA);
	m_LinesColors.push_back(colorB);
}

//=================================================================================
void C_DebugDraw::DrawLines(const std::vector<glm::vec4>& pairs, const Colours::T_Colour& color)
{
	m_LinesVertices.insert(m_LinesVertices.end(), pairs.begin(), pairs.end());
	m_LinesColors.insert(m_LinesColors.end(), pairs.size(), color);
}

//=================================================================================
void C_DebugDraw::DrawPose(const Renderer::C_Skeleton& skeleton, const Renderer::I_Pose& pose, const glm::mat4& mat)
{
	// auto transforms = pose.GetLocalSpaceTransofrms();
	// glm::vec4  zero(0.f, 0.f, 0.f, 1.f);
	//
	// const Renderer::S_Joint& rootJoint = skeleton.GetRoot();
	// std::function<void(const Renderer::S_Joint&, const glm::mat4&, const glm::vec4)> drawChildren;
	// drawChildren = [&](const Renderer::S_Joint& joint, const glm::mat4& parent, const glm::vec4& pos)
	// {
	// 	for (const auto& child : joint.m_Children)
	// 	{
	// 		const auto		fullTransform = parent * transforms[child.m_Id];
	// 		const glm::vec4 childPos	  = fullTransform * zero;
	// 		DrawLine(pos, childPos, Colours::cyan);
	// 		drawChildren(child, fullTransform, childPos);
	// 	}
	// };
	// drawChildren(rootJoint, mat * transforms[rootJoint.m_Id], mat * transforms[rootJoint.m_Id] * zero);
}

//=================================================================================
void C_DebugDraw::DrawAxis(const glm::vec3& origin, const glm::vec3& up, const glm::vec3& forward, const glm::mat4& modelMatrix)
{
	const glm::vec3 rightVec		   = toVec4(glm::normalize(glm::cross(glm::vec3(up), glm::vec3(forward))));
	const auto		originInModelSpace = modelMatrix * glm::vec4(origin, 1.0f);
	DrawLine(originInModelSpace, modelMatrix * glm::vec4((origin + forward), 1.0f), Colours::blue);
	DrawLine(originInModelSpace, modelMatrix * glm::vec4((origin + up), 1.0f), Colours::green);
	DrawLine(originInModelSpace, modelMatrix * glm::vec4((origin + rightVec), 1.0f), Colours::red);
}

//=================================================================================
void C_DebugDraw::DrawMergedGeoms()
{
	auto& shdManager		 = Shaders::C_ShaderManager::Instance();
	auto  mergedDebugProgram = shdManager.GetProgram(s_MergedShaderName);
	auto  octahedronProgram	 = shdManager.GetProgram(s_OctahedronShaderName);

	if (!m_OctahedronInfos.empty() && !m_OctahedronMesh)
	{
		m_OctahedronMesh = std::make_shared<Mesh::C_StaticMeshResource>(Renderer::MeshData::C_Geometry::CreateOctahedron(1.f, 1.f));
	}
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>([&]() { glDisable(GL_DEPTH_TEST); }, "Disable depth test."));

	auto& tgmg = Textures::C_TextureUnitManger::Instance();

	shdManager.ActivateShader(octahedronProgram);
	renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>([octahedronProgram]() { octahedronProgram->SetUniform("colorMap", 0); }));
	std::for_each(m_OctahedronInfos.begin(), m_OctahedronInfos.end(), [&](auto& info) {
		const auto modelMatrix = glm::scale(glm::translate(info.m_Position), glm::vec3(info.m_size));

		tgmg.BindTextureToUnit(*info.m_Texture.get(), 0);
		renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>([octahedronProgram, modelMatrix]() { octahedronProgram->SetUniform("modelMatrix", modelMatrix); }));
		renderer.AddCommand(std::make_unique<Commands::HACK::C_DrawStaticMesh>(m_OctahedronMesh));
	});
	m_OctahedronInfos.clear();
	shdManager.DeactivateShader();

	shdManager.ActivateShader(mergedDebugProgram);
	renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&]() {
			m_VAOlines.bind();
			std::vector<glm::vec3> mergedVertices(m_LinesVertices);
			std::vector<glm::vec3> mergedColors(m_LinesColors);
			mergedVertices.insert(mergedVertices.end(), m_PointsVertices.begin(), m_PointsVertices.end());
			mergedColors.insert(mergedColors.end(), m_PointsColors.begin(), m_PointsColors.end());
			m_VAOlines.SetBufferData<0, GL_ARRAY_BUFFER>(mergedVertices, true);
			m_VAOlines.SetBufferData<1, GL_ARRAY_BUFFER>(mergedColors, true);

			const auto lineVertices	  = static_cast<GLsizei>(m_LinesVertices.size());
			const auto pointsVertices = static_cast<GLsizei>(m_PointsVertices.size());
			if (lineVertices > 0)
			{
				glDrawArrays(GL_LINES, 0, lineVertices);
			}
			if (pointsVertices)
			{
				glPointSize(5.0f);
				glEnable(GL_PROGRAM_POINT_SIZE);
				glDrawArrays(GL_POINTS, lineVertices, pointsVertices);
			}


			m_LinesVertices.clear();
			m_LinesColors.clear();

			m_PointsVertices.clear();
			m_PointsColors.clear();
		},
		"C_DebugDraw::DrawMergedGeoms"));
	shdManager.DeactivateShader();
	auto AABBProgram = shdManager.GetProgram(s_DebugShaderName);
	shdManager.ActivateShader(AABBProgram);

	renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[this]() {
			m_VAOaabb.bind();
			m_VAOaabb.BindBuffer<1>();
		},
		"Prepare AABB"));

	for (int i = 0; i < m_AABBTransform.size(); ++i)
	{
		renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
			[this, AABBProgram, i]() {
				AABBProgram->SetUniform("modelMatrix", m_AABBTransform[i]);
				AABBProgram->SetUniform("colorIN", m_AABBColor[i]);

				glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
				glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
				glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
			},
			"Debug - DrawAABB"));
	}

	renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[this]() {
			m_VAOaabb.unbind();
			m_AABBTransform.clear();
			m_AABBColor.clear();
		},
		"Clear AABB"));

	shdManager.DeactivateShader();
	renderer.AddCommand(std::make_unique<Commands::C_GLEnable>(Commands::C_GLEnable::E_GLEnableValues::DEPTH_TEST));
}

//=================================================================================
void C_DebugDraw::ProbeDebug(const glm::vec3& position, float size, std::shared_ptr<Textures::C_Texture> texture)
{
	m_OctahedronInfos.emplace_back(OctahedronInfo{texture, size, position});
}

#endif
} // namespace GLEngine::GLRenderer