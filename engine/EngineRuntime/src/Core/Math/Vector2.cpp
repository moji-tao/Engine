#include <cmath>
#include <cassert>
#include "EngineRuntime/include/Core/Math/Vector2.h"
#include "EngineRuntime/include/Core/Math/Math.h"

namespace Engine
{
	const Vector2 Vector2::ZERO(0, 0);
	const Vector2 Vector2::UNIT_X(1, 0);
	const Vector2 Vector2::UNIT_Y(0, 1);
	const Vector2 Vector2::NEGATIVE_UNIT_X(-1, 0);
	const Vector2 Vector2::NEGATIVE_UNIT_Y(0, -1);
	const Vector2 Vector2::UNIT_SCALE(1, 1);

	Vector2::Vector2() : m_Value{ 0.0f, 0.0f } { };

	Vector2::Vector2(float _x, float _y) : m_Value{ _x,_y } { }

	Vector2::Vector2(float scaler) : m_Value{ scaler, scaler } { }

	Vector2::Vector2(const float v[2]) : m_Value{ v[0], v[1] } { }

	float* Vector2::ptr()
	{
		return m_Value;
	}

	const float* Vector2::ptr() const
	{
		return m_Value;
	}

	float Vector2::GetX() const
	{
		return m_Value[0];
	}

	float Vector2::GetY() const
	{
		return m_Value[1];
	}

	void Vector2::SetX(float scaler)
	{
		m_Value[0] = scaler;
	}

	void Vector2::SetY(float scaler)
	{
		m_Value[1] = scaler;
	}

	float Vector2::Length() const
	{
		return std::hypot(m_Value[0], m_Value[1]);
	}

	float Vector2::SquaredLength() const
	{
		return m_Value[0] * m_Value[0] + m_Value[1] * m_Value[1];
	}

	float Vector2::Distance(const Vector2& rhs) const
	{
		return (*this - rhs).Length();
	}

	float Vector2::SquaredDistance(const Vector2& rhs) const
	{
		return (*this - rhs).SquaredLength();
	}

	float Vector2::DotProduct(const Vector2& rhs) const
	{
		return m_Value[0] * rhs.m_Value[0] + m_Value[1] * rhs.m_Value[1];
	}

	float Vector2::Normalise()
	{
		float length = std::hypot(m_Value[0], m_Value[1]);

		if (length > 0.0f)
		{
			float inv_length = 1.0f / length;
			m_Value[0] *= inv_length;
			m_Value[1] *= inv_length;
		}

		return length;
	}

	Vector2 Vector2::MidPoint(const Vector2& vec) const
	{
		return Vector2((m_Value[0] + vec.m_Value[0]) * 0.5f, (m_Value[1] + vec.m_Value[1]) * 0.5f);
	}

	void Vector2::MakeFloor(const Vector2& cmp)
	{
		if (cmp.m_Value[0] < m_Value[0])
			m_Value[0] = cmp.m_Value[0];

		if (cmp.m_Value[1] < m_Value[1])
			m_Value[1] = cmp.m_Value[1];
	}

	void Vector2::MakeCeil(const Vector2& cmp)
	{
		if (cmp.m_Value[0] > m_Value[0])
			m_Value[0] = cmp.m_Value[0];

		if (cmp.m_Value[1] > m_Value[1])
			m_Value[1] = cmp.m_Value[1];
	}

	float Vector2::crossProduct(const Vector2& rhs) const
	{
		return m_Value[0] * rhs.m_Value[1] - m_Value[1] * rhs.m_Value[0];
	}

	bool Vector2::IsZeroLength() const
	{
		float sqlen = m_Value[0] * m_Value[0] + m_Value[1] * m_Value[1];
		return (sqlen < (Float_EPSILON* Float_EPSILON));
	}

	Vector2 Vector2::GetNormalised() const
	{
		Vector2 ans(*this);
		ans.Normalise();
		return ans;
	}

	Vector2 Vector2::Reflect(const Vector2& normal) const
	{
		return Vector2(*this - (2 * this->DotProduct(normal) * normal));
	}

	bool Vector2::IsNaN() const
	{
		return std::isnan(m_Value[0]) || std::isnan(m_Value[1]);
	}

	Vector2 Vector2::Lerp(const Vector2& lhs, const Vector2& rhs, float alpha)
	{
		return lhs + alpha * (rhs - lhs);
	}

	float Vector2::operator[](size_t i) const
	{
		assert(i < 2);
		return m_Value[i];
	}

	float& Vector2::operator[](size_t i)
	{
		assert(i < 2);
		return m_Value[i];
	}

	bool Vector2::operator==(const Vector2& rhs) const
	{
		return m_Value[0] == rhs.m_Value[0] && m_Value[1] == rhs.m_Value[1];
	}

	bool Vector2::operator!=(const Vector2& rhs) const
	{
		return !operator==(rhs);
	}

	Vector2 Vector2::operator+(const Vector2& rhs) const
	{
		return Vector2(m_Value[0] + rhs.m_Value[0], m_Value[1] + rhs.m_Value[1]);
	}

	Vector2 Vector2::operator-(const Vector2& rhs) const
	{
		return Vector2(m_Value[0] - rhs.m_Value[0], m_Value[1] - rhs.m_Value[1]);
	}

	Vector2 Vector2::operator*(const Vector2& rhs) const
	{
		return Vector2(m_Value[0] * rhs.m_Value[0], m_Value[1] * rhs.m_Value[1]);
	}

	Vector2 Vector2::operator*(float scaler) const
	{
		return Vector2(m_Value[0] * scaler, m_Value[1] * scaler);
	}

	Vector2 Vector2::operator/(const Vector2& rhs) const
	{
		assert(rhs.m_Value[0] != 0.0f && rhs.m_Value[1] != 0.0f);

		return Vector2(m_Value[0] / rhs.m_Value[0], m_Value[1] / rhs.m_Value[1]);
	}

	Vector2 Vector2::operator/(float scaler) const
	{
		assert(scaler != 0.0f);

		return Vector2(m_Value[0] / scaler, m_Value[1] / scaler);
	}

	bool Vector2::operator<(const Vector2& rhs) const
	{
		return m_Value[0] < rhs.m_Value[0] && m_Value[1] < rhs.m_Value[1];
	}

	bool Vector2::operator>(const Vector2& rhs) const
	{
		return m_Value[0] > rhs.m_Value[0] && m_Value[1] > rhs.m_Value[1];
	}

	const Vector2& Vector2::operator+() const
	{
		return *this;
	}

	Vector2 Vector2::operator-() const
	{
		return Vector2(-m_Value[0], -m_Value[1]);
	}

	Vector2 operator*(float scalar, const Vector2& rhs)
	{
		return Vector2(scalar * rhs.m_Value[0], scalar * rhs.m_Value[1]);
	}

	Vector2 operator/(float scalar, const Vector2& rhs)
	{
		return Vector2(scalar / rhs.m_Value[0], scalar / rhs.m_Value[1]);
	}

	Vector2 operator+(const Vector2& lhs, float rhs)
	{
		return Vector2(lhs.m_Value[0] + rhs, lhs.m_Value[1] + rhs);
	}

	Vector2 operator+(float lhs, const Vector2& rhs)
	{
		return Vector2(rhs.m_Value[0] + lhs, rhs.m_Value[1] + lhs);
	}

	Vector2 operator-(const Vector2& lhs, float rhs)
	{
		return Vector2(lhs.m_Value[0] - rhs, lhs.m_Value[1] - rhs);
	}

	Vector2 operator-(float lhs, const Vector2& rhs)
	{
		return Vector2(lhs - rhs.m_Value[0], lhs - rhs.m_Value[1]);
	}

	Vector2& Vector2::operator+=(const Vector2& rhs)
	{
		m_Value[0] += rhs.m_Value[0];
		m_Value[1] += rhs.m_Value[1];
		return *this;
	}

	Vector2& Vector2::operator+=(float scalar)
	{
		m_Value[0] += scalar;
		m_Value[1] += scalar;
		return *this;
	}

	Vector2& Vector2::operator-=(const Vector2& rhs)
	{
		m_Value[0] -= rhs.m_Value[0];
		m_Value[1] -= rhs.m_Value[1];
		return *this;
	}

	Vector2& Vector2::operator-=(float scalar)
	{
		m_Value[0] -= scalar;
		m_Value[1] -= scalar;
		return *this;
	}

	Vector2& Vector2::operator*=(float scalar)
	{
		m_Value[0] *= scalar;
		m_Value[1] *= scalar;
		return *this;
	}

	Vector2& Vector2::operator*=(const Vector2& rhs)
	{
		m_Value[0] *= rhs.m_Value[0];
		m_Value[1] *= rhs.m_Value[1];
		return *this;
	}

	Vector2& Vector2::operator/=(float scalar)
	{
		m_Value[0] /= scalar;
		m_Value[1] /= scalar;
		return *this;
	}

	Vector2& Vector2::operator/=(const Vector2& rhs)
	{
		m_Value[0] /= rhs.m_Value[0];
		m_Value[1] /= rhs.m_Value[1];
		return *this;
	}
}
