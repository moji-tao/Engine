#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Vector2.h"
#include "EngineRuntime/include/Core/Math/Quaternion.h"
#include "EngineRuntime/include/Core/Math/Angle.h"
#include "EngineRuntime/include/Core/Math/Math.h"
#include "EngineRuntime/include/Core/Meta/Reflection.h"
#include "EngineRuntime/include/Core/Base/macro.h"

namespace Engine
{
	REGISTER_CLASS(Vector3);
	REGISTER_CLASS_FIELD(Vector3, x, float);
	REGISTER_CLASS_FIELD(Vector3, y, float);
	REGISTER_CLASS_FIELD(Vector3, z, float);

	const Vector3 Vector3::ZERO(0, 0, 0);
	const Vector3 Vector3::UNIT_X(1, 0, 0);
	const Vector3 Vector3::UNIT_Y(0, 1, 0);
	const Vector3 Vector3::UNIT_Z(0, 0, 1);
	const Vector3 Vector3::NEGATIVE_UNIT_X(-1, 0, 0);
	const Vector3 Vector3::NEGATIVE_UNIT_Y(0, -1, 0);
	const Vector3 Vector3::NEGATIVE_UNIT_Z(0, 0, -1);
	const Vector3 Vector3::UNIT_SCALE(1, 1, 1);

	Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f) { }

	Vector3::Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }

	Vector3::Vector3(const float coords[3]) : x(coords[0]), y(coords[1]), z(coords[2]) { }

	Vector3::Vector3(const Vector2& vec) : x(vec.x), y(vec.y), z(0.0f) { }

	float* Vector3::ptr()
	{
		return &x;
	}

	const float* Vector3::ptr() const
	{
		return &x;
	}

	float Vector3::GetX() const
	{
		return x;
	}

	float Vector3::GetY() const
	{
		return y;
	}

	float Vector3::GetZ() const
	{
		return z;
	}

	void Vector3::SetX(float scaler)
	{
		x = scaler;
	}

	void Vector3::SetY(float scaler)
	{
		y = scaler;
	}

	void Vector3::SetZ(float scaler)
	{
		z = scaler;
	}

	float Vector3::Length() const
	{
		return std::hypot(x, y, z);
	}

	float Vector3::SquaredLength() const
	{
		return x * x + y * y + z * z;
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
		return x * vec.x + y * vec.y + z * vec.z;
	}

	void Vector3::Normalise()
	{
		float length = std::hypot(x, y, z);
		if (length > 0.0f)
		{
			float inv_length = 1.0f / length;
			x *= inv_length;
			y *= inv_length;
			z *= inv_length;
		}
	}

	Vector3 Vector3::CrossProduct(const Vector3& rhs) const
	{
		return Vector3(
			y * rhs.z - z * rhs.y,
			z * rhs.x - x * rhs.z,
			x * rhs.y - y * rhs.x);
	}

	void Vector3::MakeFloor(const Vector3& cmp)
	{
		if (cmp.x < x)
			x = cmp.x;
		if (cmp.y < y)
			y = cmp.y;
		if (cmp.z < z)
			z = cmp.z;
	}

	void Vector3::MakeCeil(const Vector3& cmp)
	{
		if (cmp.x > x)
			x = cmp.x;
		if (cmp.y > y)
			y = cmp.y;
		if (cmp.z > z)
			z = cmp.z;
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
		float sqlen = (x * x) + (y * y) + (z * z);
		return (sqlen < (1e-06 * 1e-06));
	}

	bool Vector3::IsZero() const
	{
		return x == 0.0f && y == 0.0f && z == 0.0f;
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
		return Vector3(fabsf(x), fabsf(y), fabsf(z));
	}

	Vector3 Vector3::Lerp(const Vector3& lhs, const Vector3& rhs, float alpha)
	{
		return lhs + (rhs - lhs) * alpha;
	}

	Vector3 Vector3::Clamp(const Vector3& v, const Vector3& min, const Vector3& max)
	{
		return Vector3(std::clamp(v.x, min.x, max.x),
			std::clamp(v.y, min.y, max.y),
			std::clamp(v.z, min.z, max.z));
	}

	float Vector3::GetMaxElement(const Vector3& v)
	{
		return *std::max_element(&v.x, &v.x + 3);
	}

	bool Vector3::IsNaN() const
	{
		return std::isnan(x) || std::isnan(y) || std::isnan(z);
	}

	float Vector3::operator[](size_t i) const
	{
		ASSERT(i < 3);
		return *(&x + i);
	}

	float& Vector3::operator[](size_t i)
	{
		ASSERT(i < 3);
		return *(&x + i);
	}

	bool Vector3::operator==(const Vector3& rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}

	bool Vector3::operator!=(const Vector3& rhs) const
	{
		return x != rhs.x || y != rhs.y || z != rhs.z;
	}

	Vector3 Vector3::operator+(const Vector3& rhs) const
	{
		return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Vector3 Vector3::operator-(const Vector3& rhs) const
	{
		return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Vector3 Vector3::operator*(float scalar) const
	{
		return Vector3(x * scalar, y * scalar, z * scalar);
	}

	Vector3 Vector3::operator*(const Vector3& rhs) const
	{
		return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
	}

	Vector3 Vector3::operator/(float scalar) const
	{
		ASSERT(scalar != 0.0f);
		return Vector3(x / scalar, y / scalar, z / scalar);
	}

	Vector3 Vector3::operator/(const Vector3& rhs) const
	{
		ASSERT(rhs.x != 0.0f && rhs.y != 0.0f && rhs.z != 0.0f);
		return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
	}

	const Vector3& Vector3::operator+() const
	{
		return *this;
	}

	Vector3 Vector3::operator-() const
	{
		return Vector3(-x, -y, -z);
	}

	Vector3 operator*(float scalar, const Vector3& rhs)
	{
		return Vector3(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z);
	}

	Vector3 operator/(float scalar, const Vector3& rhs)
	{
		ASSERT(rhs.x != 0.0f && rhs.y != 0.0f && rhs.z != 0.0f);
		return Vector3(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z);
	}

	Vector3 operator+(const Vector3& lhs, float rhs)
	{
		return Vector3(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
	}

	Vector3 operator+(float lhs, const Vector3& rhs)
	{
		return Vector3(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z);
	}

	Vector3 operator-(const Vector3& lhs, float rhs)
	{
		return Vector3(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs);
	}

	Vector3 operator-(float lhs, const Vector3& rhs)
	{
		return Vector3(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z);
	}

	Vector3& Vector3::operator+=(const Vector3& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	Vector3& Vector3::operator+=(float scalar)
	{
		x += scalar;
		y += scalar;
		z += scalar;
		return *this;
	}

	Vector3& Vector3::operator-=(const Vector3& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	Vector3& Vector3::operator-=(float scalar)
	{
		x -= scalar;
		y -= scalar;
		z -= scalar;
		return *this;
	}

	Vector3& Vector3::operator*=(const Vector3& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}

	Vector3& Vector3::operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	Vector3& Vector3::operator/=(const Vector3& rhs)
	{
		ASSERT(rhs.x != 0.0f && rhs.y != 0.0f && rhs.z != 0.0f);
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		return *this;
	}

	Vector3& Vector3::operator/=(float scalar)
	{
		ASSERT(scalar != 0.0f);
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}
}
