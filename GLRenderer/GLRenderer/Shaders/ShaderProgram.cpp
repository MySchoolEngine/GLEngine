#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UniformBuffer.h>
#include <GLRenderer/Commands/Shaders/GLUseProgram.h>
#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/Textures/Texture.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

namespace GLEngine { namespace GLRenderer { namespace Shaders {

//=================================================================================
C_ShaderProgram::C_ShaderProgram(GLuint program)
	: m_Program(program)
#if _DEBUG
#endif
{
	GLE_ASSERT(program != 0, "Invalid shader program");
}

//=================================================================================
C_ShaderProgram::C_ShaderProgram(C_ShaderProgram&& rhs)
	: m_bIsActive(rhs.m_bIsActive)
#if _DEBUG
	, m_name(std::move(rhs.m_name))
	, m_LastUpdate(std::move(rhs.m_LastUpdate))
	, m_Paths(std::move(rhs.m_Paths))
#endif
{
	DestroyProgram();
	m_Program	  = rhs.m_Program;
	rhs.m_Program = 0;
}

//=================================================================================
void C_ShaderProgram::operator=(C_ShaderProgram&& rhs)
{
#if _DEBUG
	SetName(rhs.m_name);
	m_LastUpdate = std::move(rhs.m_LastUpdate);
	m_Paths		 = std::move(rhs.m_Paths);
#endif
	DestroyProgram();
	m_Program	= rhs.m_Program;
	m_bIsActive = rhs.m_bIsActive;

	rhs.m_Program = 0;
}

//=================================================================================
C_ShaderProgram::~C_ShaderProgram()
{
	DestroyProgram();
}

//=================================================================================
void C_ShaderProgram::BindSampler(const Textures::C_Texture& texture, unsigned int unit)
{
	glBindSampler(unit, texture.GetTexture());
}

//=================================================================================
void C_ShaderProgram::BindSampler(const Textures::C_Texture& texture, const std::string& samplerName)
{
	BindSampler(texture, FindLocation<const std::string&>(samplerName));
}

#if _DEBUG
//=================================================================================
void C_ShaderProgram::SetPaths(std::vector<std::filesystem::path>&& paths)
{
	m_Paths		 = std::move(paths);
	m_LastUpdate = GetLastUpdate();
}

//=================================================================================
bool C_ShaderProgram::IsExpired() const
{
	return m_LastUpdate < GetLastUpdate();
}

//=================================================================================
std::filesystem::file_time_type C_ShaderProgram::GetLastUpdate() const
{
	const auto newest
		= std::max_element(m_Paths.begin(), m_Paths.end(), [](const auto a, const auto b) { return std::filesystem::last_write_time(a) < std::filesystem::last_write_time(b); });
	if (newest == m_Paths.end())
	{
		return m_LastUpdate;
	}
	return std::filesystem::last_write_time(*newest);
}
#endif

//=================================================================================
void C_ShaderProgram::useProgram()
{
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(std::move(std::make_unique<Commands::C_GLUseProgram>(m_Program)));
	m_bIsActive = true;
}

//=================================================================================
void C_ShaderProgram::disableProgram()
{
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(std::move(std::make_unique<Commands::C_GLUseProgram>(0)));
	m_bIsActive = false;
}

//=================================================================================
void C_ShaderProgram::DestroyProgram()
{
	if (m_Program != 0)
	{
		glDeleteProgram(m_Program);
		CORE_LOG(E_Level::Info, E_Context::Render, "Deleting program: {}", m_Program);
	}
}

//=================================================================================
void C_ShaderProgram::BindUBO(std::shared_ptr<Buffers::C_UniformBuffer> ubo) const
{
	int uboBlockLocation = FindUniformBlockLocation(ubo->GetBlockName().c_str());
	if (uboBlockLocation > 0)
	{
		glUniformBlockBinding(m_Program, uboBlockLocation, ubo->GetBinding());
	}
}

#if _DEBUG
//=================================================================================
void C_ShaderProgram::SetName(const std::string& name) noexcept
{
	m_name = name;
	glObjectLabel(GL_PROGRAM, m_Program, static_cast<GLsizei>(name.length()), name.c_str());
}
#endif

}}} // namespace GLEngine::GLRenderer::Shaders