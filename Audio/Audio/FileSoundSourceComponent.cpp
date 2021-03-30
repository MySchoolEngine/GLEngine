#include <AudioStdAfx.h>

#include <Audio/AudioSystemManager.h>
#include <Audio/FileSoundSourceComponent.h>

#include <GUI/FileDialogWindow.h>
#include <GUI/GUIManager.h>

#include <Core/CoreRegistrations.h>

#include <Utils/Parsing/MatrixParse.h>

#include <pugixml.hpp>

#include <fmod.hpp>
#include <imgui.h>

namespace GLEngine::Audio {

//=================================================================================
C_FileSoundSourceComponent::C_FileSoundSourceComponent(std::shared_ptr<Entity::I_Entity> owner, const std::filesystem::path& file)
	: T_Base(std::move(owner))
	, m_Filepath(file)
	, m_Sound(nullptr)
{
	SetMusic(file);
}

//=================================================================================
void C_FileSoundSourceComponent::SetMusic(const std::filesystem::path& path)
{
	m_Sound = C_AudioSystemManager::Instance().GetSoundFile(path, true);
}

//=================================================================================
void C_FileSoundSourceComponent::DebugDrawGUI(GUI::C_GUIManager* guiMGR /*= nullptr*/)
{
	ImGui::Text("%s", fmt::format("File: {}", m_Filepath).c_str());
	ImGui::SameLine();
	if (guiMGR && ImGui::Button("Choose file"))
	{
		const auto soundSelectWindowGUID = NextGUID();
		auto*	   levelSelectWindwo	 = new GUI::C_FileDialogWindow(
			 ".ogg,.mp3", "Select file",
			 [&, soundSelectWindowGUID, guiMGR](const std::filesystem::path& sound) {
				 SetMusic(sound);
				 guiMGR->DestroyWindow(soundSelectWindowGUID);
			 },
			 soundSelectWindowGUID, "./Sound");
		guiMGR->AddCustomWindow(levelSelectWindwo);
		levelSelectWindwo->SetVisible();
	}
	T_Base::DebugDrawGUI(guiMGR);
}

//=================================================================================
[[nodiscard]] std::string_view C_FileSoundSourceComponent::GetDebugComponentName() const
{
	return "File sound source";
}

//=================================================================================
FMOD::Sound* C_FileSoundSourceComponent::GetSound() const
{
	return m_Sound;
}

//=================================================================================
//=================================================================================
class C_SoundSourceBuilder : public Entity::I_ComponenetBuilder {
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

		auto listener = std::make_shared<Audio::C_FileSoundSourceComponent>(owner, file);
		listener->SetComponentMatrix(Utils::Parsing::C_MatrixParser::ParseTransformation(node));
		if (auto loopAttrib = node.attribute("looped"))
		{
			listener->SetLooped(loopAttrib.as_bool());
		}

		return listener;
	}
};

REGISTER_GLOBAL_COMPONENT_BUILDER("AudioSource", C_SoundSourceBuilder);

} // namespace GLEngine::Audio
