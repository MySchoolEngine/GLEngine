/** ==============================================
 * @file 		RenderDocTools.h
 * @date 		2018/03/17 19:55
 * @project 	Computer Graphics Project
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 *
 * @brief Helper functions for OpenGL to RenderDoc information passing.
 *
 ** ==============================================*/
 
#pragma once

namespace GLEngine {
namespace GLRenderer {
namespace RenderDoc {
/** ==============================================
 * @class C_DebugScope
 *
 * @brief	Pushes new scope context on stack for better RenderDoc debugging.
 *
 * @note	This object should be allocated on stack.
 * @todo	This should not make it's way to release version
 *
 * @author 	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * @date 	2018/03/17
 ** ==============================================*/
class C_DebugScope {
public:
	explicit C_DebugScope(const std::string& scopeName);
	~C_DebugScope();
};
}}}