#include <AudioStdAfx.h>

#include <Audio/GradrigoSoundSourceComponent.h>

#include <Core/CoreRegistrations.h>

#include <Utils/Parsing/MatrixParse.h>

#include <pugixml.hpp>

#include <fmod.hpp>
#include <imgui.h>

namespace GLEngine::Audio {


//=================================================================================
C_GradrigoSoundSourceComponent::C_GradrigoSoundSourceComponent(std::shared_ptr<Entity::I_Entity> owner)
	: T_Base(std::move(owner))
{
}

//=================================================================================
void C_GradrigoSoundSourceComponent::SetSourceFile(const std::filesystem::path& filepath)
{
	m_Filepath = filepath;
}

//=================================================================================
FMOD::Sound* C_GradrigoSoundSourceComponent::GetSound() const
{
	return nullptr;
}

//=================================================================================
void C_GradrigoSoundSourceComponent::DebugDrawGUI(GUI::C_GUIManager* guiMGR /*= nullptr*/)
{
	ImGui::Text("%s", fmt::format("Loaded file: {}", m_Filepath).c_str());
	T_Base::DebugDrawGUI(guiMGR);
}

//=================================================================================
[[nodiscard]] std::string_view C_GradrigoSoundSourceComponent::GetDebugComponentName() const
{
	return "Gradrigo sound source";
}

//=================================================================================
//=================================================================================
class C_GradrgioSoundSourceBuilder : public Entity::I_ComponenetBuilder {
public:
	virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner) override
	{
		std::filesystem::path file = "";
		if (auto fileAttrib = node.attribute("file"))
		{
			file = fileAttrib.value();
		}
		else
		{
			CORE_LOG(E_Level::Warning, E_Context::Audio, "Audio source without a sound");
		}

		auto gradrigoComponent = std::make_shared<Audio::C_GradrigoSoundSourceComponent>(owner);
		gradrigoComponent->SetSourceFile(file);
		gradrigoComponent->SetComponentMatrix(Utils::Parsing::C_MatrixParser::ParseTransformation(node));
		if (auto loopAttrib = node.attribute("looped"))
		{
			gradrigoComponent->SetLooped(loopAttrib.as_bool());
		}

		return gradrigoComponent;
	}
};

REGISTER_GLOBAL_COMPONENT_BUILDER("GradrigoAudioSource", C_GradrgioSoundSourceBuilder);

} // namespace GLEngine::Audio
