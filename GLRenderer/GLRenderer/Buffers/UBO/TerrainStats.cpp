#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/TerrainStats.h>

namespace GLEngine {
namespace GLRenderer {
namespace Buffers {
namespace UBO {

//=================================================================================
C_TerrainStats::C_TerrainStats(unsigned int index)
	: C_ShaderStorageBuffer(index)
	, min(std::numeric_limits<float>::min())
	, max(std::numeric_limits<float>::min())
	, avg(0.f)
{
	constexpr auto floatSize = sizeof(float);

	constexpr auto bytes = floatSize * 3;
	bind();
	glBufferData(GetBufferType(), bytes, nullptr, GL_DYNAMIC_DRAW);
	unbind();
}

//=================================================================================
void C_TerrainStats::UploadData() const
{
	bind();
	float* data = (float *)glMapBuffer(GetBufferType(), GL_READ_WRITE);

	data[0] = max;
	data[1] = min;
	data[2] = avg;

	glUnmapBuffer(GetBufferType());
	unbind();
}

//=================================================================================
void C_TerrainStats::DownloadData()
{
	bind();
	float* data = (float *)glMapBuffer(GetBufferType(), GL_READ_ONLY);

	max = data[0];
	min = data[1];
	avg = data[2];

	glUnmapBuffer(GetBufferType());
	unbind();
}

//=================================================================================
void C_TerrainStats::ClearBuffer()
{
	constexpr auto floatSize = sizeof(float);

	bind();
	glClearBufferData(GetBufferType(), GL_R32UI, GL_RED, GL_UNSIGNED_INT, nullptr);
	//	glClearBufferSubData(GetBufferType(), GL_R32UI, 0, bytes, GL_RED, GL_UNSIGNED_INT, &m_samples);
	unbind();
}

}}}}