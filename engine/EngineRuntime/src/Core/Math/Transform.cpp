#include "EngineRuntime/include/Core/Math/Transform.h"
#include "EngineRuntime/include/Core/Meta/Reflection.h"

namespace Engine
{
	REGISTER_CLASS(Transform);
	REGISTER_CLASS_FIELD(Transform, position, Vector3);
	REGISTER_CLASS_FIELD(Transform, scale, Vector3);
	REGISTER_CLASS_FIELD(Transform, rotation, Quaternion);
}
