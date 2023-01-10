#pragma once
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Quaternion.h"
#include "EngineRuntime/include/Core/Math/Matrix4x4.h"

namespace Engine
{
	class Transform
	{
	public:
		Transform() = default;
		Transform(const Vector3& position, const Quaternion& rotation, const Vector3& scale) :
			position{ position }, scale{ scale }, rotation{ rotation }
		{}

		Matrix4x4 getMatrix() const
		{
			Matrix4x4 temp;
			temp.MakeTransform(position, scale, rotation);
			return temp;
		}

	public:
		Vector3 position{ Vector3::ZERO };

		Vector3 scale{ Vector3::UNIT_SCALE };

		Quaternion rotation{ Quaternion::IDENTITY };
	};
}
