#pragma once

#include <Entity/World.h>

namespace GLEngine::Entity {

template <class T, class... Args> T& C_Entity::Add(Args&&... args)
{
	return m_World->Registry().emplace<T>(m_Handle, std::forward<Args>(args)...);
}

template <class T> T& C_Entity::Get()
{
	return m_World->Registry().get<T>(m_Handle);
}

template <class T> const T& C_Entity::Get() const
{
	return m_World->Registry().get<T>(m_Handle);
}

template <class T> T* C_Entity::TryGet()
{
	return m_World->Registry().try_get<T>(m_Handle);
}

template <class T> bool C_Entity::Has() const
{
	return m_World->Registry().all_of<T>(m_Handle);
}

template <class T> void C_Entity::Remove()
{
	m_World->Registry().remove<T>(m_Handle);
}

} // namespace GLEngine::Entity
