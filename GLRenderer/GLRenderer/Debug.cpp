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

#include <Renderer/Animation/Skeleton.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Mesh/Geometry.h>

#include <Physics/Primitives/Frustum.h>

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
		break;
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
		break;
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
void C_DebugDraw::DrawBone(const glm::vec3& position, const Renderer::Animation::S_Joint& joint)
{
	const auto		locTransformation = glm::inverse((joint.m_InverseBindTransfomr));
	const glm::vec4 modelDest		  = (locTransformation * glm::vec4(0.f, 0.f, 0.0f, 1.f));
	const glm::vec3 dest			  = glm::vec3(modelDest / modelDest.w);
	const glm::vec3 boneOffset		  = dest - position;

	const float bumpFactor	 = .33f;
	const auto	BumpPosition = position + boneOffset * bumpFactor;

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

	tangent				 = glm::normalize(tangent);
	const auto bitangent = glm::cross(tangent, boneOffset);

	const float		bumpSize = .05f;
	const glm::vec3 Offset1	 = tangent * bumpSize;
	const glm::vec3 Offset2	 = bitangent * bumpSize;


	DrawLine(position, BumpPosition, Colours::white);
	DrawLine(BumpPosition, dest, Colours::green);

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
	const auto		locTransformation = glm::inverse((skeleton.m_Root->m_InverseBindTransfomr));
	const glm::vec4 modelDest		  = (locTransformation * glm::vec4(0.f, 0.f, 0.0f, 1.f));
	const glm::vec3 dest			  = glm::vec3(modelDest / modelDest.w);
	const glm::vec3 boneOffset		  = dest - root;

	for (const auto& child : skeleton.m_Root->m_Children)
	{
		DrawBone(root + boneOffset, child);
	}
}

//=================================================================================
void C_DebugDraw::DrawAxis(const glm::vec3& origin, const glm::vec3& up, const glm::vec3& foreward, const glm::mat4& modelMatrix)
{
	glm::vec3  rightVec			  = toVec4(glm::normalize(glm::cross(glm::vec3(up), glm::vec3(foreward))));
	const auto originInModelSpace = modelMatrix * glm::vec4(origin, 1.0f);
	DrawLine(originInModelSpace, modelMatrix * glm::vec4((origin + foreward), 1.0f), Colours::blue);
	DrawLine(originInModelSpace, modelMatrix * glm::vec4((origin + up), 1.0f), Colours::green);
	DrawLine(originInModelSpace, modelMatrix * glm::vec4((origin + rightVec), 1.0f), Colours::red);
}

//=================================================================================
void C_DebugDraw::DrawGrid(const glm::vec4& origin, unsigned short linesToSide, const glm::mat4& modelMatrix /*= glm::mat4(1.0f)*/)
{
	int limit = linesToSide;
	// cross for center
	DrawLine(origin + glm::vec4(-limit, 0, 0, 1.f), origin + glm::vec4(limit, 0, 0, 1.f));
	DrawLine(origin + glm::vec4(0, 0, -limit, 1.f), origin + glm::vec4(0, 0, limit, 1.f));

	for (int i = 1; i <= limit; ++i)
	{
		// lines in positive direction from origin
		DrawLine(origin + glm::vec4(-limit, 0, i, 1.f), origin + glm::vec4(limit, 0, i, 1.f));
		DrawLine(origin + glm::vec4(i, 0, -limit, 1.f), origin + glm::vec4(i, 0, limit, 1.f));

		// lines in negative direction from origin
		DrawLine(origin + glm::vec4(-limit, 0, -i, 1.f), origin + glm::vec4(limit, 0, -i, 1.f));
		DrawLine(origin + glm::vec4(-i, 0, -limit, 1.f), origin + glm::vec4(-i, 0, limit, 1.f));
	}
}

//=================================================================================
void C_DebugDraw::DrawFrustum(const Physics::Primitives::C_Frustum& frust, const Colours::T_Colour& color)
{
	const auto& position = frust.GetPosition();
	const auto& upVector = frust.GetUpVector();
	const auto& forward	 = frust.GetForeward();
	const auto	fnear	 = frust.GetNear();
	const auto	ffar	 = frust.GetFar();
	const auto	fov		 = frust.GetFov();
	const auto	aspect	 = frust.GetAspect();

	const glm::vec3 nearCenter = glm::vec3(position + (forward * fnear));
	const glm::vec3 farCenter  = glm::vec3(position + (forward * ffar));

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
	lines.push_back(nlt);
	lines.push_back(nrt);
	lines.push_back(nlb);
	lines.push_back(nrb);
	lines.push_back(nlt);
	lines.push_back(nlb);
	lines.push_back(nrt);
	lines.push_back(nrb);

	lines.push_back(flt);
	lines.push_back(frt);
	lines.push_back(flb);
	lines.push_back(frb);
	lines.push_back(flt);
	lines.push_back(flb);
	lines.push_back(frt);
	lines.push_back(frb);

	lines.push_back(nlt);
	lines.push_back(flt);
	lines.push_back(nrt);
	lines.push_back(frt);
	lines.push_back(nlb);
	lines.push_back(flb);
	lines.push_back(nrb);
	lines.push_back(frb);

	DrawLines(lines, color);
	DrawPoint(position);
	DrawLine(position, position + forward, Colours::green);
	DrawLine(position, position + upVector, Colours::red);
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
			m_VAOlines.SetBufferData<0, GL_ARRAY_BUFFER>(mergedVertices, Renderer::BufferDynamics::Dynamic);
			m_VAOlines.SetBufferData<1, GL_ARRAY_BUFFER>(mergedColors, Renderer::BufferDynamics::Dynamic);

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
	auto AABBprogram = shdManager.GetProgram(s_DebugShaderName);
	shdManager.ActivateShader(AABBprogram);

	renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[this]() {
			m_VAOaabb.bind();
			m_VAOaabb.BindBuffer<1>();
		},
		"Prepare AABB"));

	for (int i = 0; i < m_AABBTransform.size(); ++i)
	{
		renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
			[this, AABBprogram, i]() {
				AABBprogram->SetUniform("modelMatrix", m_AABBTransform[i]);
				AABBprogram->SetUniform("colorIN", m_AABBColor[i]);

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