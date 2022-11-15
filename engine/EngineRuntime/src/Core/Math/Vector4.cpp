#include "EngineRuntime/include/Core/Math/Vector4.h"
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include <cassert>
#include <cmath>

namespace Engine
{
	const Vector4 Vector4::ZERO(0, 0, 0, 0);

	const Vector4 Vector4::UNIT_SCALE(1.0f, 1.0f, 1.0f, 1.0f);

	Vector4::Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }

	Vector4::Vector4(const Vector3& v3, float _w) : x(v3.x), y(v3.y), z(v3.z), w(_w) { }

	Vector4::Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) { }

	Vector4::Vector4(float coords[4]) : x(coords[0]), y(coords[1]), z(coords[2]), w(coords[3]) { }

	float* Vector4::ptr()
	{
		return &x;
	}

	const float* Vector4::ptr() const
	{
		return &x;
	}

	float Vector4::GetX() const
	{
		return x;
	}

	float Vector4::GetY() const
	{
		return y;
	}

	float Vector4::GetZ() const
	{
		return z;
	}

	float Vector4::GetW() const
	{
		return w;
	}

	void Vector4::SetX(float scaler)
	{
		x = scaler;
	}

	void Vector4::SetY(float scaler)
	{
		y = scaler;
	}

	void Vector4::SetZ(float scaler)
	{
		z = scaler;
	}

	void Vector4::SetW(float scaler)
	{
		w = scaler;
	}

	float Vector4::DotProduct(const Vector4& vec) const
	{
		return x * vec[0] + y * vec[1] + z * vec[2] + w * vec[3];
	}

	bool Vector4::IsNaN() const
	{
		return std::isnan(x) || std::isnan(y) || std::isnan(z) || std::isnan(w);
	}

	float Vector4::operator[](size_t i) const
	{
		assert(i < 4);
		return *(&x + i);
	}

	float& Vector4::operator[](size_t i)
	{
		assert(i < 4);
		return *(&x + i);
	}

	Vector4& Vector4::operator=(float scalar)
	{
		x = scalar;
		y = scalar;
		z = scalar;
		w = scalar;
		return *this;
	}

	bool Vector4::operator==(const Vector4& rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
	}

	bool Vector4::operator!=(const Vector4& rhs) const
	{
		return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
	}

	Vector4 Vector4::operator+(const Vector4& rhs) const
	{
		return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}

	Vector4 Vector4::operator-(const Vector4& rhs) const
	{
		return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}

	Vector4 Vector4::operator*(float scalar) const
	{
		return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
	}

	Vector4 Vector4::operator*(const Vector4& rhs) const
	{
		return Vector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
	}

	Vector4 Vector4::operator/(float scalar) const
	{
		assert(scalar != 0.0f);
		return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
	}

	Vector4 Vector4::operator/(const Vector4& rhs) const
	{
		assert(rhs.x != 0.0f && rhs.y != 0.0f && rhs.z != 0.0f && rhs.w != 0.0f);
		return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
	}

	const Vector4& Vector4::operator+() const
	{
		return *this;
	}

	Vector4 Vector4::operator-() const
	{
		return Vector4(-x, -y, -z, -w);
	}

	Vector4 operator*(float scalar, const Vector4& rhs)
	{
		return Vector4(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z, scalar * rhs.w);
	}

	Vector4 operator/(float scalar, const Vector4& rhs)
	{
		assert(rhs.x != 0.0f && rhs.y != 0.0f && rhs.z != 0.0f && rhs.w != 0.0f);
		return Vector4(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z, scalar / rhs.w);
	}

	Vector4 operator+(const Vector4& lhs, float rhs)
	{
		return Vector4(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs);
	}

	Vector4 operator+(float lhs, const Vector4& rhs)
	{
		return Vector4(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w);
	}

	Vector4 operator-(const Vector4& lhs, float rhs)
	{
		return Vector4(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs);
	}

	Vector4 operator-(float lhs, const Vector4& rhs)
	{
		return Vector4(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w);
	}

	Vector4& Vector4::operator+=(const Vector4& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;
		return *this;
	}

	Vector4& Vector4::operator-=(const Vector4& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;
		return *this;
	}

	Vector4& Vector4::operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
	}

	Vector4& Vector4::operator+=(float scalar)
	{
		x += scalar;
		y += scalar;
		z += scalar;
		w += scalar;
		return *this;
	}

	Vector4& Vector4::operator-=(float scalar)
	{
		x -= scalar;
		y -= scalar;
		z -= scalar;
		w -= scalar;
		return *this;
	}

	Vector4& Vector4::operator*=(const Vector4& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		w *= rhs.w;
		return *this;
	}

	Vector4& Vector4::operator/=(float scalar)
	{
		assert(scalar != 0.0f);
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;
		return *this;
	}

	Vector4& Vector4::operator/=(const Vector4& rhs)
	{
		assert(rhs.x != 0.0f && rhs.y != 0.0f && rhs.z != 0.0f && rhs.w != 0.0f);
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		w /= rhs.w;
		return *this;
	}
}
