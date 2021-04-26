#pragma once

#include <Audio/SoundSourceComponent.h>

namespace GLEngine::Audio {

class C_FileSoundSourceComponent : public C_SoundSourceComponent {
	using T_Base = C_SoundSourceComponent;

public:
	C_FileSoundSourceComponent(std::shared_ptr<Entity::I_Entity> owner, const std::filesystem::path& file);

	void SetMusic(const std::filesystem::path& path);

	virtual void						   DebugDrawGUI(GUI::C_GUIManager* guiMGR = nullptr) override;
	virtual [[nodiscard]] std::string_view GetDebugComponentName() const override;

protected:
	[[nodiscard]] virtual FMOD::Sound* GetSound() const override;

private:
	std::filesystem::path m_Filepath;
	FMOD::Sound*		  m_Sound;
};

} // namespace GLEngine::Audio
