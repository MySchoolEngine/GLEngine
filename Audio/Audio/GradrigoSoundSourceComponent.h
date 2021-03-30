#pragma once

#include <Audio/SoundSourceComponent.h>

namespace GLEngine::Audio {

class C_GradrigoSoundSourceComponent : public C_SoundSourceComponent {
	using T_Base = C_SoundSourceComponent;

public:
	C_GradrigoSoundSourceComponent(std::shared_ptr<Entity::I_Entity> owner);

	void SetSourceFile(const std::filesystem::path& filepath);

	virtual void						   DebugDrawGUI(GUI::C_GUIManager* guiMGR = nullptr) override;
	virtual [[nodiscard]] std::string_view GetDebugComponentName() const override;

protected:
	[[nodiscard]] virtual FMOD::Sound* GetSound() const override;

private:
	std::filesystem::path m_Filepath;
};

} // namespace GLEngine::Audio