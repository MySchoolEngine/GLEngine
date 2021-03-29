#pragma once

#include <Entity/ComponentBuilder.h>
#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

namespace FMOD {
class System;
class Sound;
class Channel;
} // namespace FMOD

namespace GLEngine::Audio {

class AUDIO_API_EXPORT C_SoundSourceComponent : public Entity::I_Component {
public:
	C_SoundSourceComponent(std::shared_ptr<Entity::I_Entity> owner, const std::filesystem::path& file);

	[[nodiscard]] glm::vec3 GetPosition() const;

	virtual void Update() override;

	void PlaySound();

	virtual [[nodiscard]] Entity::E_ComponentType	  GetType() const override;
	virtual [[nodiscard]] bool						  HasDebugDrawGUI() const override;
	virtual [[nodiscard]] std::string_view			  GetDebugComponentName() const override;
	virtual [[nodiscard]] Physics::Primitives::S_AABB GetAABB() const override;
	virtual void									  DebugDrawGUI() override;

protected:
	[[nodiscard]] virtual FMOD::Sound* GetSound() const;

	FMOD::Channel* m_Channel;
	bool		   m_Looped;

private:
	void				  UpdateSoundSourcePosition();
	std::filesystem::path m_Filepath;
	FMOD::Sound*		  m_Sound;
};

} // namespace GLEngine::Audio

namespace GLEngine {
template <> class ComponenetBase<Entity::E_ComponentType::AudioSource> {
public:
	using type = Audio::C_SoundSourceComponent;
};
} // namespace GLEngine