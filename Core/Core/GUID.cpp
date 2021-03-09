#include <CoreStdafx.h>

#include <Core/GUID.h>

namespace GLEngine {

static GUID nextGUID = 1;

//=================================================================================
GUID NextGUID()
{
	return nextGUID++;
}

} // namespace GLEngine