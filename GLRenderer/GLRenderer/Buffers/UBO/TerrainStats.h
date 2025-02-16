#pragma once

#include <GLRenderer/Buffers/ShaderStorage.h>


namespace GLEngine::GLRenderer::Buffers::UBO {

class C_TerrainStats : public C_ShaderStorageBuffer {
public:
	explicit C_TerrainStats(unsigned int index);
	~C_TerrainStats() override = default;


	float min;
	float max;
	float avg;
	//==============================================
	// C_ShaderStorageBuffer
	//==============================================
	void UploadData() const override;
	void DownloadData() override;
	void ClearBuffer() override;
};

} // namespace GLEngine::GLRenderer::Buffers::UBO