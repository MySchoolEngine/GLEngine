#include <GLRendererStdafx.h>

#include <GLRenderer/PersistentDebug.h>


namespace GLEngine {
namespace GLRenderer {

//=================================================================================
C_PersistentDebug& C_PersistentDebug::Instance()
{
	static C_PersistentDebug    instance; // Guaranteed to be destroyed.
									// Instantiated on first use.
	return instance;
}

}}