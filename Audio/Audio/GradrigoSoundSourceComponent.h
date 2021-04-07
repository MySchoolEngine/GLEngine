#pragma once

#include <Audio/SoundSourceComponent.h>

namespace GLEngine::Audio {

struct FMOD_SOUND;

class C_GradrigoSoundSourceComponent : public C_SoundSourceComponent {
	using T_Base = C_SoundSourceComponent;

public:
	C_GradrigoSoundSourceComponent(std::shared_ptr<Entity::I_Entity> owner);
	~C_GradrigoSoundSourceComponent();

	void SetSourceFile(const std::filesystem::path& filepath);

	virtual void						   DebugDrawGUI(GUI::C_GUIManager* guiMGR = nullptr) override;
	virtual [[nodiscard]] std::string_view GetDebugComponentName() const override;

	FMOD_RESULT GetBuffer(void* data, unsigned int datalen);

protected:
	[[nodiscard]] virtual FMOD::Sound* GetSound() const override;
	void							   CheckForParseResponse();


private:
	FMOD::Sound*		  m_Sound;
	std::filesystem::path m_Filepath;
	int					  m_Gradrigo;
	std::optional<int>	  m_ParseRequest;
};
} // namespace GLEngine::Audio