#include "EngineRuntime/include/Core/Math/Vector4.h"
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include <cassert>
#include <cmath>

namespace Engine
{
	const Vector4 Vector4::ZERO(0, 0, 0, 0);

	const Vector4 Vector4::UNIT_SCALE(1.0f, 1.0f, 1.0f, 1.0f);

	Vector4::Vector4() : m_Value{ 0.0f, 0.0f, 0.0f, 0.0f } { }

	Vector4::Vector4(const Vector3& v3, float w) : m_Value{ v3[0], v3[1], v3[2], w } { }

	Vector4::Vector4(float x, float y, float z, float w) : m_Value{ x, y, z, w } { }

	Vector4::Vector4(float coords[4]) : m_Value{ coords[0], coords[1], coords[2], coords[3] } { }

	float* Vector4::ptr()
	{
		return (float*)m_Value;
	}

	const float* Vector4::ptr() const
	{
		return (const float*)m_Value;
	}

	float Vector4::GetX() const
	{
		return m_Value[0];
	}

	float Vector4::GetY() const
	{
		return m_Value[1];
	}

	float Vector4::GetZ() const
	{
		return m_Value[2];
	}

	float Vector4::GetW() const
	{
		return m_Value[3];
	}

	void Vector4::SetX(float scaler)
	{
		m_Value[0] = scaler;
	}

	void Vector4::SetY(float scaler)
	{
		m_Value[1] = scaler;
	}

	void Vector4::SetZ(float scaler)
	{
		m_Value[2] = scaler;
	}

	void Vector4::SetW(float scaler)
	{
		m_Value[3] = scaler;
	}

	float Vector4::DotProduct(const Vector4& vec) const
	{
		return m_Value[0] * vec[0] + m_Value[1] * vec[1] + m_Value[2] * vec[2] + m_Value[3] * vec[3];
	}

	bool Vector4::IsNaN() const
	{
		return std::isnan(m_Value[0]) || std::isnan(m_Value[1]) || std::isnan(m_Value[2]) || std::isnan(m_Value[3]);
	}

	float Vector4::operator[](size_t i) const
	{
		assert(i < 4);
		return m_Value[i];
	}

	float& Vector4::operator[](size_t i)
	{
		assert(i < 4);
		return m_Value[i];
	}

	Vector4& Vector4::operator=(float scalar)
	{
		m_Value[0] = scalar;
		m_Value[1] = scalar;
		m_Value[2] = scalar;
		m_Value[3] = scalar;
		return *this;
	}

	bool Vector4::operator==(const Vector4& rhs) const
	{
		return m_Value[0] == rhs.m_Value[0] && m_Value[1] == rhs.m_Value[1] && m_Value[2] == rhs.m_Value[2] && m_Value[3] == rhs.m_Value[3];
	}

	bool Vector4::operator!=(const Vector4& rhs) const
	{
		return m_Value[0] != rhs.m_Value[0] || m_Value[1] != rhs.m_Value[1] || m_Value[2] != rhs.m_Value[2] || m_Value[3] != rhs.m_Value[3];
	}

	Vector4 Vector4::operator+(const Vector4& rhs) const
	{
		return Vector4(m_Value[0] + rhs.m_Value[0], m_Value[1] + rhs.m_Value[1], m_Value[2] + rhs.m_Value[2], m_Value[3] + rhs.m_Value[3]);
	}

	Vector4 Vector4::operator-(const Vector4& rhs) const
	{
		return Vector4(m_Value[0] - rhs.m_Value[0], m_Value[1] - rhs.m_Value[1], m_Value[2] - rhs.m_Value[2], m_Value[3] - rhs.m_Value[3]);
	}

	Vector4 Vector4::operator*(float scalar) const
	{
		return Vector4(m_Value[0] * scalar, m_Value[1] * scalar, m_Value[2] * scalar, m_Value[3] * scalar);
	}

	Vector4 Vector4::operator*(const Vector4& rhs) const
	{
		return Vector4(m_Value[0] * rhs.m_Value[0], m_Value[1] * rhs.m_Value[1], m_Value[2] * rhs.m_Value[2], m_Value[3] * rhs.m_Value[3]);
	}

	Vector4 Vector4::operator/(float scalar) const
	{
		assert(scalar != 0.0f);
		return Vector4(m_Value[0] / scalar, m_Value[1] / scalar, m_Value[2] / scalar, m_Value[3] / scalar);
	}

	Vector4 Vector4::operator/(const Vector4& rhs) const
	{
		assert(rhs.m_Value[0] != 0.0f && rhs.m_Value[1] != 0.0f && rhs.m_Value[2] != 0.0f && rhs.m_Value[3] != 0.0f);
		return Vector4(m_Value[0] / rhs.m_Value[0], m_Value[1] / rhs.m_Value[1], m_Value[2] / rhs.m_Value[2], m_Value[3] / rhs.m_Value[3]);
	}

	const Vector4& Vector4::operator+() const
	{
		return *this;
	}

	Vector4 Vector4::operator-() const
	{
		return Vector4(-m_Value[0], -m_Value[1], -m_Value[2], -m_Value[3]);
	}

	Vector4 operator*(float scalar, const Vector4& rhs)
	{
		return Vector4(scalar * rhs.m_Value[0], scalar * rhs.m_Value[1], scalar * rhs.m_Value[2], scalar * rhs.m_Value[3]);
	}

	Vector4 operator/(float scalar, const Vector4& rhs)
	{
		assert(rhs.m_Value[0] != 0.0f && rhs.m_Value[1] != 0.0f && rhs.m_Value[2] != 0.0f && rhs.m_Value[3] != 0.0f);
		return Vector4(scalar / rhs.m_Value[0], scalar / rhs.m_Value[1], scalar / rhs.m_Value[2], scalar / rhs.m_Value[3]);
	}

	Vector4 operator+(const Vector4& lhs, float rhs)
	{
		return Vector4(lhs.m_Value[0] + rhs, lhs.m_Value[1] + rhs, lhs.m_Value[2] + rhs, lhs.m_Value[3] + rhs);
	}

	Vector4 operator+(float lhs, const Vector4& rhs)
	{
		return Vector4(lhs + rhs.m_Value[0], lhs + rhs.m_Value[1], lhs + rhs.m_Value[2], lhs + rhs.m_Value[3]);
	}

	Vector4 operator-(const Vector4& lhs, float rhs)
	{
		return Vector4(lhs.m_Value[0] - rhs, lhs.m_Value[1] - rhs, lhs.m_Value[2] - rhs, lhs.m_Value[3] - rhs);
	}

	Vector4 operator-(float lhs, const Vector4& rhs)
	{
		return Vector4(lhs - rhs.m_Value[0], lhs - rhs.m_Value[1], lhs - rhs.m_Value[2], lhs - rhs.m_Value[3]);
	}

	Vector4& Vector4::operator+=(const Vector4& rhs)
	{
		m_Value[0] += rhs.m_Value[0];
		m_Value[1] += rhs.m_Value[1];
		m_Value[2] += rhs.m_Value[2];
		m_Value[3] += rhs.m_Value[3];
		return *this;
	}

	Vector4& Vector4::operator-=(const Vector4& rhs)
	{
		m_Value[0] -= rhs.m_Value[0];
		m_Value[1] -= rhs.m_Value[1];
		m_Value[2] -= rhs.m_Value[2];
		m_Value[3] -= rhs.m_Value[3];
		return *this;
	}

	Vector4& Vector4::operator*=(float scalar)
	{
		m_Value[0] *= scalar;
		m_Value[1] *= scalar;
		m_Value[2] *= scalar;
		m_Value[3] *= scalar;
		return *this;
	}

	Vector4& Vector4::operator+=(float scalar)
	{
		m_Value[0] += scalar;
		m_Value[1] += scalar;
		m_Value[2] += scalar;
		m_Value[3] += scalar;
		return *this;
	}

	Vector4& Vector4::operator-=(float scalar)
	{
		m_Value[0] -= scalar;
		m_Value[1] -= scalar;
		m_Value[2] -= scalar;
		m_Value[3] -= scalar;
		return *this;
	}

	Vector4& Vector4::operator*=(const Vector4& rhs)
	{
		m_Value[0] *= rhs.m_Value[0];
		m_Value[1] *= rhs.m_Value[1];
		m_Value[2] *= rhs.m_Value[2];
		m_Value[3] *= rhs.m_Value[3];
		return *this;
	}

	Vector4& Vector4::operator/=(float scalar)
	{
		assert(scalar != 0.0f);
		m_Value[0] /= scalar;
		m_Value[1] /= scalar;
		m_Value[2] /= scalar;
		m_Value[3] /= scalar;
		return *this;
	}

	Vector4& Vector4::operator/=(const Vector4& rhs)
	{
		assert(rhs.m_Value[0] != 0.0f && rhs.m_Value[1] != 0.0f && rhs.m_Value[2] != 0.0f && rhs.m_Value[3] != 0.0f);
		m_Value[0] /= rhs.m_Value[0];
		m_Value[1] /= rhs.m_Value[1];
		m_Value[2] /= rhs.m_Value[2];
		m_Value[3] /= rhs.m_Value[3];
		return *this;
	}
}
