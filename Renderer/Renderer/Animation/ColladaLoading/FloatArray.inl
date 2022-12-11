#pragma once

namespace GLEngine::Renderer {

//=================================================================================
template <> inline float S_FloatArray::Get<float>()
{
	float val;
	m_FloatsStream >> val;
	m_Position += 1;
	return val;
}

//=================================================================================
template <> inline glm::vec2 S_FloatArray::Get<glm::vec2>()
{
	float f1, f2;
	m_FloatsStream >> f1 >> f2;
	m_Position += 2;
	return glm::vec2(f1, f2);
}

//=================================================================================
template <> inline glm::vec3 S_FloatArray::Get<glm::vec3>()
{
	float f1, f2, f3;
	m_FloatsStream >> f1 >> f2 >> f3;
	m_Position += 3;
	return glm::vec3(f1, f2, f3);
}

//=================================================================================
template <> inline glm::vec4 S_FloatArray::Get<glm::vec4>()
{
	float f1, f2, f3, f4;
	m_FloatsStream >> f1 >> f2 >> f3 >> f4;
	m_Position += 4;
	return glm::vec4(f1, f2, f3, f4);
}

//=================================================================================
template <> inline glm::mat4 S_FloatArray::Get<glm::mat4>()
{
	float f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16;

	m_FloatsStream >> f1 >> f2 >> f3 >> f4 >> f5 >> f6 >> f7 >> f8 >> f9 >> f10 >> f11 >> f12 >> f13 >> f14 >> f15 >> f16;

	m_Position += 16;
	return glm::mat4(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16);
}
template <> inline std::size_t S_FloatArray::count<float>() const
{
	return m_NumFloats;
}
template <> inline std::size_t S_FloatArray::count<glm::vec2>() const
{
	return m_NumFloats / 2;
}
template <> inline std::size_t S_FloatArray::count<glm::vec3>() const
{
	return m_NumFloats / 3;
}
template <> inline std::size_t S_FloatArray::count<glm::vec4>() const
{
	return m_NumFloats / 4;
}
template <> inline std::size_t S_FloatArray::count<glm::mat4>() const
{
	return m_NumFloats / 16;
}

} // namespace GLEngine::Renderer::Animation