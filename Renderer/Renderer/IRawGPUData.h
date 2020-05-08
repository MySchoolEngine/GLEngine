#pragma once

#include <memory>

namespace GLEngine {
namespace Renderer {

class I_Resource;

/** ==============================================
 * @class I_RawGPUDAta
 *
 * @brief	This is raw data extracted via I_Extractor. This class exists to
 *			keep data out of entity to avoid semafors as much as possible.
 *			Data will be than transformed into GPU friendly data, but this
 *			will happen when simulation thread can run again.
 *
 * @author 	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * @date 	2019/03/06
 ** ==============================================*/
class I_RawGPUData {
public:
	explicit I_RawGPUData(std::shared_ptr<I_Resource> res)
		: m_Resource(res) {}
	virtual ~I_RawGPUData() = default;
	/** ==============================================
	 * @method:    Size
	 * @return:    std::size_t
	 * @brief	   It is here for statistics. Should return # of bytes it
	 *			   takes after extraction.
	 ** ==============================================*/
	virtual std::size_t Size() const = 0;
	[[nodiscard]] std::shared_ptr<I_Resource> GetResource() const { return m_Resource; }

protected:
	std::shared_ptr<I_Resource> m_Resource;
};

}
}