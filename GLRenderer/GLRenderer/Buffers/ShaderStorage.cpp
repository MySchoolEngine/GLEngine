#include "ShaderStorage.h"
#include <algorithm>    // std::min_element, std::max_element

namespace GLW {

	//=================================================================================
	C_ShaderStorageBuffer::C_ShaderStorageBuffer(int bindingPoint)
		: m_BindingPoint(bindingPoint)
	{

	}

	//=================================================================================
	void C_ShaderStorageBuffer::bind()
	{
		C_GLBuffer<GL_SHADER_STORAGE_BUFFER>::bind();
		const_cast<C_ShaderStorageBuffer*>(this)->glBindBufferBase(GetBufferType(), m_BindingPoint, m_id);
	}

}

////=================================================================================
//// C_Histogram
////=================================================================================
//C_Histogram::C_Histogram(unsigned int samples, int bindingPoint)
//	: m_samples(samples)
//	, C_ShaderStorageBuffer(bindingPoint)
//{
//	ErrorCheck();
//	bind();
//	glBufferData(GetBufferType(), (m_samples + 4)*sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);
//	ErrorCheck();
//}
//
////=================================================================================
//void C_Histogram::UploadData() const
//{
//	bind();
//	unsigned int* data = (unsigned int *)glMapBuffer(GetBufferType(), GL_READ_WRITE);
//
//	std::vector<unsigned int> v( 265, 0 );
//
//	data = v.data();
//
//	glUnmapBuffer(GetBufferType());
//}
//
////=================================================================================
//void C_Histogram::DownloadData()
//{
//	bind();
//	int* data = (int *)glMapBuffer(GetBufferType(), GL_READ_WRITE);
//
//	m_histogram = std::vector<unsigned int>(data, data + m_samples);
//	m_highestValue = data[m_samples];
//	m_lowestIndex = data[m_samples + 1];
//	m_highestIndex = data[m_samples + 2];
//
//	glUnmapBuffer(GetBufferType());
//}
//
////=================================================================================
//glm::uvec2 C_Histogram::FindLimits() const
//{
//	glm::uvec2 ret(m_samples, 0);
//	for (unsigned int i = 0; i < m_samples; ++i) {
//		const int sample = m_histogram[i];
//		if (sample == 0) continue;
//		if (i < ret.x) {
//			ret.x = i;
//		}
//		if (i > ret.y) {
//			ret.y = i;
//		}
//	}
//	return ret;
//}
//
////=================================================================================
//int C_Histogram::GetHighest() const
//{
//	return *std::max_element(m_histogram.cbegin(), m_histogram.cend());
//}
//
////=================================================================================
//void C_Histogram::ClearBuffer()
//{
//	bind();
//	glClearBufferData(GetBufferType(), GL_R32UI, GL_RED, GL_UNSIGNED_INT, nullptr);
//	glClearBufferSubData(GetBufferType(), GL_R32UI, (m_samples+1)*sizeof(GLuint), sizeof(GLuint), GL_RED, GL_UNSIGNED_INT, &m_samples);
//}
//
//#pragma region C_SplitPlanesStorage
////=================================================================================
//// C_SplitPlanesStorage
////=================================================================================
//C_SplitPlanesStorage::C_SplitPlanesStorage(unsigned int numFrustums, int bindingPoint)
//	: m_NumFrustums(numFrustums)
//	, C_ShaderStorageBuffer(bindingPoint)
//{
//	//m_Frustums.reserve(m_NumFrustums);
//	for (unsigned int i = 0; i < m_NumFrustums; ++i) {
//		m_Frustums.emplace_back(0.0f, 0.0f);
//	}
//	bind();
//	glBufferData(GetBufferType(), m_NumFrustums*2 * sizeof(decltype (m_Frustums)::value_type), nullptr, GL_DYNAMIC_DRAW);
//	ErrorCheck();
//	unbind();
//}
////=================================================================================
//void C_SplitPlanesStorage::UploadData() const
//{
//	bind();
//	decltype (m_Frustums)::value_type* data = (decltype (m_Frustums)::value_type*)glMapBuffer(GetBufferType(), GL_READ_WRITE);
//
//	memcpy(data, m_Frustums.data(), m_NumFrustums * 2 * sizeof(decltype (m_Frustums)::value_type));
//
//	glUnmapBuffer(GetBufferType());
//	unbind();
//}
//
////=================================================================================
//void C_SplitPlanesStorage::DownloadData()
//{
//	bind();
//	decltype (m_Frustums)::value_type* data = (decltype (m_Frustums)::value_type *)glMapBuffer(GetBufferType(), GL_READ_WRITE);
//
//	memcpy(m_Frustums.data(), data, m_NumFrustums * sizeof(decltype (m_Frustums)::value_type));
//
//	glUnmapBuffer(GetBufferType());
//	unbind();
//}
//
////=================================================================================
//void C_SplitPlanesStorage::ClearBuffer()
//{
//	bind();
//	glClearBufferData(GetBufferType(), GL_R32UI, GL_RED, GL_UNSIGNED_INT, nullptr);
//	unbind();
//}
#pragma endregion C_SplitPlanesStorage