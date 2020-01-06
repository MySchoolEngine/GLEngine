#pragma once


namespace GLEngine::Renderer::Animation {

//=================================================================================
template<>
float S_FloatArray::Get()
{
	float val;
	m_FloatsStream >> val;
	m_Position += 1;
	return val;
}

//=================================================================================
template<>
glm::vec2 S_FloatArray::Get()
{
	float f1, f2;
	m_FloatsStream >> f1 >> f2;
	m_Position += 2;
	return glm::vec2(f1, f2);
}

//=================================================================================
template<>
glm::vec3 S_FloatArray::Get()
{
	float f1, f2, f3;
	m_FloatsStream >> f1 >> f2 >> f3;
	m_Position += 3;
	return glm::vec3(f1, f2, f3);
}

//=================================================================================
template<>
glm::vec4 S_FloatArray::Get()
{
	float f1, f2, f3, f4;
	m_FloatsStream >> f1 >> f2 >> f3 >> f4;
	m_Position += 4;
	return glm::vec4(f1, f2, f3, f4);
}

//=================================================================================
template<>
glm::mat4 S_FloatArray::Get()
{
	float f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16;

	m_FloatsStream
		>> f1  >> f2  >> f3  >> f4
		>> f5  >> f6  >> f7  >> f8
		>> f9  >> f10 >> f11 >> f12
		>> f13 >> f14 >> f15 >> f16;

	m_Position += 16;
	return glm::mat4(
		f1,  f2,  f3,  f4,
		f5,  f6,  f7,  f8,
		f9,  f10, f11, f12,
		f13, f14, f15, f16
	);
}

}