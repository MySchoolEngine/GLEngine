#include <GLRendererStdafx.h>

#include <GLRenderer/Shaders/ShaderProgram.h>

#include <GLRenderer/Buffers/UniformBuffer.h>
#include <GLRenderer/Commands/Shaders/GLUseProgram.h>
#include <GLRenderer/Textures/Texture.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

namespace GLEngine {
namespace GLRenderer {
namespace Shaders {

//=================================================================================
C_ShaderProgram::C_ShaderProgram(GLuint program)
	: m_Program(program)
#if _DEBUG
	, m_LastUpdate(std::chrono::system_clock::now())
#endif
{
	GLE_ASSERT(program != 0, "Invalid shader program");
}

//=================================================================================
C_ShaderProgram::C_ShaderProgram(C_ShaderProgram&& rhs)
	:  m_bIsActive(rhs.m_bIsActive)
#if _DEBUG
	, m_name(std::move(rhs.m_name))
	, m_LastUpdate(std::move(rhs.m_LastUpdate))
#endif
{
	DestroyProgram();
	m_Program = rhs.m_Program;
	rhs.m_Program = 0;
}

//=================================================================================
void C_ShaderProgram::operator=(C_ShaderProgram&& rhs)
{
#if _DEBUG
	SetName(rhs.m_name);
	m_LastUpdate = std::move(rhs.m_LastUpdate);
#endif
	DestroyProgram();
	m_Program	= rhs.m_Program;
	m_bIsActive	= rhs.m_bIsActive;

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
void C_ShaderProgram::SetUpdateTIme(std::chrono::system_clock::time_point update)
{
	m_LastUpdate = update;
}

//=================================================================================
std::chrono::system_clock::time_point C_ShaderProgram::GetLastUpdate() const
{
	return m_LastUpdate;
}
#endif

//=================================================================================
void C_ShaderProgram::useProgram()
{
	//Core::C_Application::Get().GetActiveRenderer()->AddCommand(
	//	std::move(
	//		std::make_unique<Commands::C_GLUseProgram>(m_Program)
	//	)
	//);
	glUseProgram(m_Program);
	m_bIsActive = true;
}

//=================================================================================
void C_ShaderProgram::disableProgram()
{
	glUseProgram(0);
	m_bIsActive = false;
}

//=================================================================================
void C_ShaderProgram::DestroyProgram()
{
	if (m_Program != 0) {
		glDeleteProgram(m_Program);
		CORE_LOG(E_Level::Info, E_Context::Render, "Deleting program: {}", m_Program);
	}
}

//=================================================================================
void C_ShaderProgram::BindUBO(std::shared_ptr<Buffers::C_UniformBuffer> ubo) const
{
	int uboBlockLocation = FindUniformBlockLocation(ubo->GetBlockName().c_str());
	if (uboBlockLocation > 0) {
		glUniformBlockBinding(m_Program, uboBlockLocation, ubo->GetBinding());
	}
}

}}}