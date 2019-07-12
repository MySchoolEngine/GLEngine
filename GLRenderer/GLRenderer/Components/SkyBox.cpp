#include <GLRendererStdafx.h>

#include <GLRenderer/Components/SkyBox.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>

#include <GLRenderer/MeshLoading/Scene.h>

#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

#include <GLRenderer/Textures/TextureLoader.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

namespace GLEngine {
namespace GLRenderer {
namespace Components {

//=================================================================================
C_SkyBox::C_SkyBox()
	: m_Textures("Skybox", GL_TEXTURE_2D_ARRAY)
{
	m_Textures.bind();
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, 512, 512, 6);
	m_Textures.unbind();
}

//=================================================================================
void C_SkyBox::AddTexture(E_Side side, std::string& filename)
{

	Textures::TextureLoader tl;
	Mesh::Texture t;
	bool retval = tl.loadTexture(filename.c_str(), t);

	if (!retval)
		CORE_LOG(E_Level::Error, E_Context::Render, "TExture cannot be loaded");


	const int mipMapLevel = 0;
	m_Textures.bind();
	const glm::ivec3 coord(0, 0, static_cast<int>(side));
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, mipMapLevel, coord.x, coord.y, coord.z, 
		512, 512, 1, GL_RGBA, T_TypeToGL<decltype(t.data)::element_type>::value, t.data.get());
	m_Textures.unbind();
}

//=================================================================================
void C_SkyBox::PerformDraw() const
{
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					glActiveTexture(GL_TEXTURE0);
					m_Textures.bind();
					}
				)
		)
	);
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					auto& shmgr = Shaders::C_ShaderManager::Instance();
					auto shader = shmgr.GetProgram("skybox");
					}
				)
		)
	);
}

}}}