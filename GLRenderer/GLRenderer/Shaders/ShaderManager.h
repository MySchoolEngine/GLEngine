/** ==============================================
 * @file 		ShaderManager.h
 * @date 		2018/03/17 19:59
 * @project 	Computer Graphics Project
 * @faculty 	Faculty of Information Technology
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/

#pragma once

#include <GLRenderer/Shaders/ShaderCompiler.h>

#include <GUI/Input/CheckBoxValue.h>
#include <GUI/LambdaPart.h>

namespace pugi {
class xml_document;
class xml_node;
} // namespace pugi

namespace GLEngine {
namespace GUI {
class C_GUIManager;
}
namespace GLRenderer { namespace Shaders {
class C_ShaderProgram;

// I just wanna have some statistics and also save some loading time and memory
/** ==============================================
 * @class C_ShaderManager
 *
 * @brief	All the shaders in application should be build through this manager.
 *
 * Holds all shaders in application, can print statistics about shaders usage.
 * Also it is possible to refresh every m_Timeout defined intervals all shaders
 * for better shader develop.
 *
 * @author 	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * @date 	2018/03/17
 ** ==============================================*/
class C_ShaderManager {
	using T_ShaderPtr = std::shared_ptr<C_ShaderProgram>;

public:
	// Singleton stuff
	C_ShaderManager(C_ShaderManager const&) = delete;
	void					operator=(C_ShaderManager const&) = delete;
	static C_ShaderManager& Instance();

	void Clear();
	/** ==============================================
	 * @method:    Update
	 * @fullName:  C_ShaderManager::Update
	 * @return:    void
	 * @brief	   You need to call this if you want to refresh shaders regularly
	 *			   as refresh is synchronous.
	 ** ==============================================*/
	void Update();

	T_ShaderPtr GetProgram(const std::string& name);
	bool		ShaderLoaded(const std::string& name);

	void ActivateShader(T_ShaderPtr shader);
	void DeactivateShader();

	std::string ShadersStatistics() const;

	GUID SetupControls(GUI::C_GUIManager& guiMGR);
	void DestroyControls(GUI::C_GUIManager& guiMGR);

private:
	C_ShaderManager();

	bool LoadDoc(pugi::xml_document& document, const std::filesystem::path& filename) const;

	GLuint LoadShader(const pugi::xml_node& node, C_ShaderCompiler& compiler) const;
	GLuint LoadProgram(const std::filesystem::path& name, C_ShaderCompiler& compiler) const;

	void ReloadProgram(const std::string& programName, std::shared_ptr<C_ShaderProgram> program) const;

	using T_ProgramMap = std::map<std::string, std::shared_ptr<C_ShaderProgram>>;

	T_ProgramMap m_Programs;

	T_ShaderPtr m_ActiveShader;

	const static std::filesystem::path s_ShadersFolder;

	std::chrono::system_clock::duration	  m_Timeout;
	std::chrono::system_clock::time_point m_LastUpdate;

	GUID							   m_Window;
	std::unique_ptr<GUI::C_LambdaPart> m_ShaderList;
	GUI::Input::C_CheckBoxValue		   m_PreprocessorOutput;
};
}} // namespace GLRenderer::Shaders
} // namespace GLEngine