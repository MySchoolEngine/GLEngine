#pragma once

#include <GLRenderer\Buffers\ShaderStorage.h>


namespace GLEngine {
namespace GLRenderer {
namespace Buffers {
namespace UBO {

class C_TerrainStats : public C_ShaderStorageBuffer {
public:
	explicit C_TerrainStats(unsigned int index);
	~C_TerrainStats() = default;


	float min;
	float max;
	float avg;
	//==============================================
	// C_ShaderStorageBuffer
	//==============================================
	virtual void UploadData() const override;
	virtual void DownloadData() override;
	virtual void ClearBuffer() override;

};

}}}}