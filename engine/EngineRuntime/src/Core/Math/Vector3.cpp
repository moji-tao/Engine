#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Quaternion.h"
#include "EngineRuntime/include/Core/Math/Angle.h"
#include "EngineRuntime/include/Core/Math/Math.h"
#include <algorithm>
#include <cassert>
#include <cmath>

namespace Engine
{
	const Vector3 Vector3::ZERO(0, 0, 0);
	const Vector3 Vector3::UNIT_X(1, 0, 0);
	const Vector3 Vector3::UNIT_Y(0, 1, 0);
	const Vector3 Vector3::UNIT_Z(0, 0, 1);
	const Vector3 Vector3::NEGATIVE_UNIT_X(-1, 0, 0);
	const Vector3 Vector3::NEGATIVE_UNIT_Y(0, -1, 0);
	const Vector3 Vector3::NEGATIVE_UNIT_Z(0, 0, -1);
	const Vector3 Vector3::UNIT_SCALE(1, 1, 1);

	Vector3::Vector3() : m_Value{ 0.0f, 0.0f, 0.0f } { }

	Vector3::Vector3(float x, float y, float z) : m_Value{ x, y, z } { }

	Vector3::Vector3(const float coords[3]) : m_Value{ coords[0], coords[1], coords[2] } { }

	float* Vector3::ptr()
	{
		return m_Value;
	}

	const float* Vector3::ptr() const
	{
		return m_Value;
	}

	float Vector3::GetX() const
	{
		return m_Value[0];
	}

	float Vector3::GetY() const
	{
		return m_Value[1];
	}

	float Vector3::GetZ() const
	{
		return m_Value[2];
	}

	void Vector3::SetX(float scaler)
	{
		m_Value[0] = scaler;
	}

	void Vector3::SetY(float scaler)
	{
		m_Value[1] = scaler;
	}

	void Vector3::SetZ(float scaler)
	{
		m_Value[2] = scaler;
	}

	float Vector3::Length() const
	{
		return std::hypot(m_Value[0], m_Value[1], m_Value[2]);
	}

	float Vector3::SquaredLength() const
	{
		return m_Value[0] * m_Value[0] + m_Value[1] * m_Value[1] + m_Value[2] * m_Value[2];
	}

	float Vector3::Distance(const Vector3& rhs) const
	{
		return (*this - rhs).Length();
	}

	float Vector3::SquaredDistance(const Vector3& rhs) const
	{
		return (*this - rhs).SquaredLength();
	}

	float Vector3::DotProduct(const Vector3& vec) const
	{
		return m_Value[0] * vec.m_Value[0] + m_Value[1] * vec.m_Value[1] + m_Value[2] * vec.m_Value[2];
	}

	void Vector3::Normalise()
	{
		float length = std::hypot(m_Value[0], m_Value[1], m_Value[2]);
		if (length > 0.0f)
		{
			float inv_length = 1.0f / length;
			m_Value[0] *= inv_length;
			m_Value[1] *= inv_length;
			m_Value[2] *= inv_length;
		}
	}

	Vector3 Vector3::CrossProduct(const Vector3& rhs) const
	{
		return Vector3(
			m_Value[1] * rhs.m_Value[2] - m_Value[2] * rhs.m_Value[1],
			m_Value[2] * rhs.m_Value[0] - m_Value[0] * rhs.m_Value[2],
			m_Value[0] * rhs.m_Value[1] - m_Value[1] * rhs.m_Value[0]);
	}

	void Vector3::MakeFloor(const Vector3& cmp)
	{
		if (cmp.m_Value[0] < m_Value[0])
			m_Value[0] = cmp.m_Value[0];
		if (cmp.m_Value[1] < m_Value[1])
			m_Value[1] = cmp.m_Value[1];
		if (cmp.m_Value[2] < m_Value[2])
			m_Value[2] = cmp.m_Value[2];
	}

	void Vector3::MakeCeil(const Vector3& cmp)
	{
		if (cmp.m_Value[0] > m_Value[0])
			m_Value[0] = cmp.m_Value[0];
		if (cmp.m_Value[1] > m_Value[1])
			m_Value[1] = cmp.m_Value[1];
		if (cmp.m_Value[2] > m_Value[2])
			m_Value[2] = cmp.m_Value[2];
	}

	Radian Vector3::GetAngle(const Vector3& dest) const
	{
		float len_product = Length() * dest.Length();

		// Divide by zero check
		if (len_product < 1e-6f)
			len_product = 1e-6f;

		float f = DotProduct(dest) / len_product;

		f = Math::Clamp(f, (float)-1.0, (float)1.0);
		return Math::Acos(f);
	}

	Quaternion Vector3::GetRotationTo(const Vector3& dest, const Vector3& fallback_axis) const
	{
		// Based on Stan Melax's article in Game Programming Gems
		Quaternion q;
		// Copy, since cannot modify local
		Vector3 v0 = *this;
		Vector3 v1 = dest;
		v0.Normalise();
		v1.Normalise();

		float d = v0.DotProduct(v1);
		// If dot == 1, vectors are the same
		if (d >= 1.0f)
		{
			return Quaternion::IDENTITY;
		}
		if (d < (1e-6f - 1.0f))
		{
			if (fallback_axis != Vector3::ZERO)
			{
				// rotate 180 degrees about the fall back axis
				q.SetDataFromAngleAxis(Radian(Math_PI), fallback_axis);
			}
			else
			{
				// Generate an axis
				Vector3 axis = Vector3::UNIT_X.CrossProduct(*this);
				if (axis.IsZeroLength()) // pick another if collinear
					axis = Vector3::UNIT_Y.CrossProduct(*this);
				axis.Normalise();
				q.SetDataFromAngleAxis(Radian(Math_PI), axis);
			}
		}
		else
		{
			float s = Math::Sqrt((1 + d) * 2);
			float invs = 1 / s;

			Vector3 c = v0.CrossProduct(v1);

			q.SetX(c.GetX() * invs);
			q.SetY(c.GetY() * invs);
			q.SetZ(c.GetZ() * invs);
			q.SetW(s * 0.5f);

			q.Normalise();
		}
		return q;

	}

	bool Vector3::IsZeroLength() const
	{
		float sqlen = (m_Value[0] * m_Value[0]) + (m_Value[1] * m_Value[1]) + (m_Value[2] * m_Value[2]);
		return (sqlen < (1e-06 * 1e-06));
	}

	bool Vector3::IsZero() const
	{
		return m_Value[0] == 0.0f && m_Value[1] == 0.0f && m_Value[2] == 0.0f;
	}

	Vector3 Vector3::GetNormalised() const
	{
		Vector3 ans = *this;
		ans.Normalise();
		return ans;
	}

	Vector3 Vector3::Reflect(const Vector3& normal) const
	{
		return Vector3(*this - (2 * this->DotProduct(normal) * normal));
	}

	Vector3 Vector3::Project(const Vector3& normal) const
	{
		return Vector3(*this - (this->DotProduct(normal) * normal));
	}

	Vector3 Vector3::GetAbsolute() const
	{
		return Vector3(fabsf(m_Value[0]), fabsf(m_Value[1]), fabsf(m_Value[2]));
	}

	Vector3 Vector3::Lerp(const Vector3& lhs, const Vector3& rhs, float alpha)
	{
		return lhs + (rhs - lhs) * alpha;
	}

	Vector3 Vector3::Clamp(const Vector3& v, const Vector3& min, const Vector3& max)
	{
		return Vector3(std::clamp(v.m_Value[0], min.m_Value[0], max.m_Value[0]),
			std::clamp(v.m_Value[1], min.m_Value[1], max.m_Value[1]),
			std::clamp(v.m_Value[2], min.m_Value[2], max.m_Value[2]));
	}

	float Vector3::GetMaxElement(const Vector3& v)
	{
		return *std::max_element(v.m_Value, v.m_Value + 3);
	}

	bool Vector3::IsNaN() const
	{
		return std::isnan(m_Value[0]) || std::isnan(m_Value[1]) || std::isnan(m_Value[2]);
	}

	float Vector3::operator[](size_t i) const
	{
		assert(i < 3);
		return m_Value[i];
	}

	float& Vector3::operator[](size_t i)
	{
		assert(i < 3);
		return m_Value[i];
	}

	bool Vector3::operator==(const Vector3& rhs) const
	{
		return m_Value[0] == rhs.m_Value[0] && m_Value[1] == rhs.m_Value[1] && m_Value[2] == rhs.m_Value[2];
	}

	bool Vector3::operator!=(const Vector3& rhs) const
	{
		return m_Value[0] != rhs.m_Value[0] || m_Value[1] != rhs.m_Value[1] || m_Value[2] != rhs.m_Value[2];
	}

	Vector3 Vector3::operator+(const Vector3& rhs) const
	{
		return Vector3(m_Value[0] + rhs.m_Value[0], m_Value[1] + rhs.m_Value[1], m_Value[2] + rhs.m_Value[2]);
	}

	Vector3 Vector3::operator-(const Vector3& rhs) const
	{
		return Vector3(m_Value[0] - rhs.m_Value[0], m_Value[1] - rhs.m_Value[1], m_Value[2] - rhs.m_Value[2]);
	}

	Vector3 Vector3::operator*(float scalar) const
	{
		return Vector3(m_Value[0] * scalar, m_Value[1] * scalar, m_Value[2] * scalar);
	}

	Vector3 Vector3::operator*(const Vector3& rhs) const
	{
		return Vector3(m_Value[0] * rhs.m_Value[0], m_Value[1] * rhs.m_Value[1], m_Value[2] * rhs.m_Value[2]);
	}

	Vector3 Vector3::operator/(float scalar) const
	{
		assert(scalar != 0.0f);
		return Vector3(m_Value[0] / scalar, m_Value[1] / scalar, m_Value[2] / scalar);
	}

	Vector3 Vector3::operator/(const Vector3& rhs) const
	{
		assert(rhs.m_Value[0] != 0.0f && rhs.m_Value[1] != 0.0f && rhs.m_Value[2] != 0.0f);
		return Vector3(m_Value[0] / rhs.m_Value[0], m_Value[1] / rhs.m_Value[1], m_Value[2] / rhs.m_Value[2]);
	}

	const Vector3& Vector3::operator+() const
	{
		return *this;
	}

	Vector3 Vector3::operator-() const
	{
		return Vector3(-m_Value[0], -m_Value[1], -m_Value[2]);
	}

	Vector3 operator*(float scalar, const Vector3& rhs)
	{
		return Vector3(scalar * rhs.m_Value[0], scalar * rhs.m_Value[1], scalar * rhs.m_Value[2]);
	}

	Vector3 operator/(float scalar, const Vector3& rhs)
	{
		assert(rhs.m_Value[0] != 0.0f && rhs.m_Value[1] != 0.0f && rhs.m_Value[2] != 0.0f);
		return Vector3(scalar / rhs.m_Value[0], scalar / rhs.m_Value[1], scalar / rhs.m_Value[2]);
	}

	Vector3 operator+(const Vector3& lhs, float rhs)
	{
		return Vector3(lhs.m_Value[0] + rhs, lhs.m_Value[1] + rhs, lhs.m_Value[2] + rhs);
	}

	Vector3 operator+(float lhs, const Vector3& rhs)
	{
		return Vector3(lhs + rhs.m_Value[0], lhs + rhs.m_Value[1], lhs + rhs.m_Value[2]);
	}

	Vector3 operator-(const Vector3& lhs, float rhs)
	{
		return Vector3(lhs.m_Value[0] - rhs, lhs.m_Value[1] - rhs, lhs.m_Value[2] - rhs);
	}

	Vector3 operator-(float lhs, const Vector3& rhs)
	{
		return Vector3(lhs - rhs.m_Value[0], lhs - rhs.m_Value[1], lhs - rhs.m_Value[2]);
	}

	Vector3& Vector3::operator+=(const Vector3& rhs)
	{
		m_Value[0] += rhs.m_Value[0];
		m_Value[1] += rhs.m_Value[1];
		m_Value[2] += rhs.m_Value[2];
		return *this;
	}

	Vector3& Vector3::operator+=(float scalar)
	{
		m_Value[0] += scalar;
		m_Value[1] += scalar;
		m_Value[2] += scalar;
		return *this;
	}

	Vector3& Vector3::operator-=(const Vector3& rhs)
	{
		m_Value[0] -= rhs.m_Value[0];
		m_Value[1] -= rhs.m_Value[1];
		m_Value[2] -= rhs.m_Value[2];
		return *this;
	}

	Vector3& Vector3::operator-=(float scalar)
	{
		m_Value[0] -= scalar;
		m_Value[1] -= scalar;
		m_Value[2] -= scalar;
		return *this;
	}

	Vector3& Vector3::operator*=(const Vector3& rhs)
	{
		m_Value[0] *= rhs.m_Value[0];
		m_Value[1] *= rhs.m_Value[1];
		m_Value[2] *= rhs.m_Value[2];
		return *this;
	}

	Vector3& Vector3::operator*=(float scalar)
	{
		m_Value[0] *= scalar;
		m_Value[1] *= scalar;
		m_Value[2] *= scalar;
		return *this;
	}

	Vector3& Vector3::operator/=(const Vector3& rhs)
	{
		assert(rhs.m_Value[0] != 0.0f && rhs.m_Value[1] != 0.0f && rhs.m_Value[2] != 0.0f);
		m_Value[0] /= rhs.m_Value[0];
		m_Value[1] /= rhs.m_Value[1];
		m_Value[2] /= rhs.m_Value[2];
		return *this;
	}

	Vector3& Vector3::operator/=(float scalar)
	{
		assert(scalar != 0.0f);
		m_Value[0] /= scalar;
		m_Value[1] /= scalar;
		m_Value[2] /= scalar;
		return *this;
	}
}
