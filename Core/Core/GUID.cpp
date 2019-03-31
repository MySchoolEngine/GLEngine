#include <CoreStdafx.h>

#include <Core/GUID.h>

namespace GLEngine {

static GUID nextGUID = 0;

//=================================================================================
GUID NextGUID()
{
	return nextGUID++;
}

}