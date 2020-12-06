#include "ShaderManager.h"

#include "GLW/ShaderProgram.h"
#include "Shaders/UniformBuffersManager.h"

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif


#include <iostream>
#include <sstream>


//=================================================================================
const std::string C_ShaderManager::s_ShadersFolder = "shaders/";

//=================================================================================
C_ShaderManager & C_ShaderManager::Instance()
{
	static C_ShaderManager    instance; // Guaranteed to be destroyed.
						  // Instantiated on first use.
	return instance;
}

//=================================================================================
void C_ShaderManager::Clear()
{
	std::cout << ShadersStatistics();
	m_Programs.clear();
	m_ActiveShader.reset();
}

//=================================================================================
void C_ShaderManager::Update()
{
	DeactivateShader();
#if _DEBUG
	const auto currentTime = std::chrono::system_clock::now();
	if (m_LastUpdate + m_Timeout < currentTime) {
		m_ActiveShader.reset();
		for (auto& program : m_Programs) {
			try
			{
				program.second.swap(std::make_shared<GLW::C_ShaderProgram>(LoadProgram(program.first)));
				C_UniformBuffersManager::Instance().ProcessUBOBindingPoints(program.second);
			}
			catch (...)
			{
				std::cerr << "Unable to reload shader " << program.first << " so we keep it outdated" << std::endl;
			}
		}
		m_LastUpdate = currentTime;
	}
#endif
}

//=================================================================================
C_ShaderManager::T_ShaderPtr C_ShaderManager::GetProgram(const std::string& name)
{
	if (ShaderLoaded(name)) {
		return m_Programs[name];
	}

	GLuint program = LoadProgram(name);

	T_ShaderPtr shaderProgram = std::make_shared<GLW::C_ShaderProgram>(program);

	C_UniformBuffersManager::Instance().ProcessUBOBindingPoints(shaderProgram);
	shaderProgram->SetName(name);

	m_Programs.insert({ name, shaderProgram});

	return shaderProgram;
}

//=================================================================================
bool C_ShaderManager::ShaderLoaded(const std::string & name)
{
	return m_Programs.find(name) != m_Programs.end();
}

//=================================================================================
void C_ShaderManager::ActivateShader(T_ShaderPtr shader)
{
	if (shader == nullptr) {
		throw 1;// just error
	}
	if (shader != m_ActiveShader || !m_ActiveShader->IsActive()) {
		m_ActiveShader = shader;
		m_ActiveShader->useProgram();
	}
}

//=================================================================================
void C_ShaderManager::DeactivateShader()
{
	// this is done because imGUI can change shader without letting us know
	if (m_ActiveShader) {
		m_ActiveShader->disableProgram();
		m_ActiveShader = nullptr;
	}
}

//=================================================================================
std::string C_ShaderManager::ShadersStatistics() const
{
	std::stringstream ss;
	ss << "Loaded shaders: " << m_Programs.size() << std::endl;
	for (auto& shader : m_Programs) {
		ss << shader.first << " used by " << shader.second.use_count() - 1 << std::endl;
		ss << "->\tHas stages:" << std::endl;
	}
	return ss.str();
}

//=================================================================================
C_ShaderManager::C_ShaderManager()
	: m_Timeout(std::chrono::seconds(1))
	, m_LastUpdate(std::chrono::system_clock::now())
	, m_Compiler()
{

}

//=================================================================================
bool C_ShaderManager::LoadDoc(pugi::xml_document & document, const std::string & filename) const
{
	pugi::xml_parse_result result;

	if (filename.find(".xml") != std::string::npos) {
		result = document.load_file(filename.c_str());
		if (!result.status == pugi::status_ok) {
			result = document.load_file((s_ShadersFolder + filename).c_str());
		}
		else {
			return true;
		}
	}
	else {
		return LoadDoc(document, filename + ".xml");
	}

	if (result.status == pugi::status_ok) return true;
	return false;
}

//=================================================================================
GLuint C_ShaderManager::LoadShader(const pugi::xml_node& node) const
{
	GLuint shader = 0;
	std::string str;

	std::string stageAttribute = node.attribute("stage").value();

	int stage = GL_VERTEX_SHADER;
	if (stageAttribute == "vertex") stage = GL_VERTEX_SHADER;
	else if (stageAttribute == "fragment") stage = GL_FRAGMENT_SHADER;
	else if (stageAttribute == "geometry") stage = GL_GEOMETRY_SHADER;
	else if (stageAttribute == "compute") stage = GL_COMPUTE_SHADER;

	if (!m_Compiler.compileShader(shader, std::string(s_ShadersFolder + std::string(node.first_child().value())).c_str(), stage, str)) {
		std::cerr << "--Compilation error" << std::endl;
		std::cerr << s_ShadersFolder + std::string(node.first_child().value()) << std::endl;
		std::cerr << str << std::endl;
		return 0;
	}
	return shader;
}

//=================================================================================
GLuint C_ShaderManager::LoadProgram(const std::string& name) const
{
	std::string filePath;

	pugi::xml_document doc;

	if (!LoadDoc(doc, name)) {
		std::cerr << __FUNCTION__ << ": Can't open config file for shader name: " << name << std::endl;
		throw;
	}

	std::vector<GLuint> shaders;

	//struct stat result;
	//if (stat(name.c_str(), &result) == 0)
	//{
	//	auto mod_time = result.st_mtime;
	//	
	//}

	for (auto& shader : doc.child("pipeline").children("shader")) {
		GLuint shaderStage = LoadShader(shader);
		if (shaderStage == 0) {
			throw;
		}
		shaders.push_back(shaderStage);
	}

	GLuint program;
	std::string str;
	if (!m_Compiler.linkProgram(program, str, shaders)) {
		std::cerr << str << std::endl;
		return false;
	}
	return program;
}
