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
			m_position{ position }, m_scale{ scale }, m_rotation{ rotation }
		{}

		Matrix4x4 getMatrix() const
		{
			Matrix4x4 temp;
			temp.MakeTransform(m_position, m_scale, m_rotation);
			return temp;
		}

	public:
		Vector3 m_position{ Vector3::ZERO };

		Vector3 m_scale{ Vector3::UNIT_SCALE };

		Quaternion m_rotation{ Quaternion::IDENTITY };
	};
}
