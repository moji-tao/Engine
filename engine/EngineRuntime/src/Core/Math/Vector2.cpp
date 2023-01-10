#include "EngineRuntime/include/Core/Math/Vector2.h"
#include "EngineRuntime/include/Core/Math/Math.h"
#include "EngineRuntime/include/Core/Meta/Reflection.h"
#include "EngineRuntime/include/Core/Base/macro.h"

namespace Engine
{
	REGISTER_CLASS(Vector2);
	REGISTER_CLASS_FIELD(Vector2, x, float);
	REGISTER_CLASS_FIELD(Vector2, y, float);

	const Vector2 Vector2::ZERO(0, 0);
	const Vector2 Vector2::UNIT_X(1, 0);
	const Vector2 Vector2::UNIT_Y(0, 1);
	const Vector2 Vector2::NEGATIVE_UNIT_X(-1, 0);
	const Vector2 Vector2::NEGATIVE_UNIT_Y(0, -1);
	const Vector2 Vector2::UNIT_SCALE(1, 1);

	Vector2::Vector2() : x(0.0f), y(0.0f) { };

	Vector2::Vector2(float _x, float _y) : x(_x), y(_y) { }

	Vector2::Vector2(float scaler) : x(scaler), y(scaler) { }

	Vector2::Vector2(const float v[2]) : x(v[0]), y(v[1]) { }

	float* Vector2::ptr()
	{
		return &x;
	}

	const float* Vector2::ptr() const
	{
		return &x;
	}

	float Vector2::GetX() const
	{
		return x;
	}

	float Vector2::GetY() const
	{
		return y;
	}

	void Vector2::SetX(float scaler)
	{
		x = scaler;
	}

	void Vector2::SetY(float scaler)
	{
		y = scaler;
	}

	float Vector2::Length() const
	{
		return std::hypot(x, y);
	}

	float Vector2::SquaredLength() const
	{
		return x * x + y * y;
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
		return x * rhs.x + y * rhs.y;
	}

	float Vector2::Normalise()
	{
		float length = std::hypot(x, y);

		if (length > 0.0f)
		{
			float inv_length = 1.0f / length;
			x *= inv_length;
			y *= inv_length;
		}

		return length;
	}

	Vector2 Vector2::MidPoint(const Vector2& vec) const
	{
		return Vector2((x + vec.x) * 0.5f, (y + vec.y) * 0.5f);
	}

	void Vector2::MakeFloor(const Vector2& cmp)
	{
		if (cmp.x < x)
			x = cmp.x;

		if (cmp.y < y)
			y = cmp.y;
	}

	void Vector2::MakeCeil(const Vector2& cmp)
	{
		if (cmp.x > x)
			x = cmp.x;

		if (cmp.y > y)
			y = cmp.y;
	}

	float Vector2::crossProduct(const Vector2& rhs) const
	{
		return x * rhs.y - y * rhs.x;
	}

	bool Vector2::IsZeroLength() const
	{
		float sqlen = x * x + y * y;
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
		return std::isnan(x) || std::isnan(y);
	}

	Vector2 Vector2::Lerp(const Vector2& lhs, const Vector2& rhs, float alpha)
	{
		return lhs + alpha * (rhs - lhs);
	}

	float Vector2::operator[](size_t i) const
	{
		ASSERT(i < 2);
		return *(&x + i);
	}

	float& Vector2::operator[](size_t i)
	{
		ASSERT(i < 2);
		return *(&x + i);
	}

	bool Vector2::operator==(const Vector2& rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}

	bool Vector2::operator!=(const Vector2& rhs) const
	{
		return !operator==(rhs);
	}

	Vector2 Vector2::operator+(const Vector2& rhs) const
	{
		return Vector2(x + rhs.x, y + rhs.y);
	}

	Vector2 Vector2::operator-(const Vector2& rhs) const
	{
		return Vector2(x - rhs.x, y - rhs.y);
	}

	Vector2 Vector2::operator*(const Vector2& rhs) const
	{
		return Vector2(x * rhs.x, y * rhs.y);
	}

	Vector2 Vector2::operator*(float scaler) const
	{
		return Vector2(x * scaler, y * scaler);
	}

	Vector2 Vector2::operator/(const Vector2& rhs) const
	{
		ASSERT(rhs.x != 0.0f && rhs.y != 0.0f);

		return Vector2(x / rhs.x, y / rhs.y);
	}

	Vector2 Vector2::operator/(float scaler) const
	{
		ASSERT(scaler != 0.0f);

		return Vector2(x / scaler, y / scaler);
	}

	bool Vector2::operator<(const Vector2& rhs) const
	{
		return x < rhs.x && y < rhs.y;
	}

	bool Vector2::operator>(const Vector2& rhs) const
	{
		return x > rhs.x && y > rhs.y;
	}

	const Vector2& Vector2::operator+() const
	{
		return *this;
	}

	Vector2 Vector2::operator-() const
	{
		return Vector2(-x, -y);
	}

	Vector2 operator*(float scalar, const Vector2& rhs)
	{
		return Vector2(scalar * rhs.x, scalar * rhs.y);
	}

	Vector2 operator/(float scalar, const Vector2& rhs)
	{
		return Vector2(scalar / rhs.x, scalar / rhs.y);
	}

	Vector2 operator+(const Vector2& lhs, float rhs)
	{
		return Vector2(lhs.x + rhs, lhs.y + rhs);
	}

	Vector2 operator+(float lhs, const Vector2& rhs)
	{
		return Vector2(rhs.x + lhs, rhs.y + lhs);
	}

	Vector2 operator-(const Vector2& lhs, float rhs)
	{
		return Vector2(lhs.x - rhs, lhs.y - rhs);
	}

	Vector2 operator-(float lhs, const Vector2& rhs)
	{
		return Vector2(lhs - rhs.x, lhs - rhs.y);
	}

	Vector2& Vector2::operator+=(const Vector2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	Vector2& Vector2::operator+=(float scalar)
	{
		x += scalar;
		y += scalar;
		return *this;
	}

	Vector2& Vector2::operator-=(const Vector2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	Vector2& Vector2::operator-=(float scalar)
	{
		x -= scalar;
		y -= scalar;
		return *this;
	}

	Vector2& Vector2::operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Vector2& Vector2::operator*=(const Vector2& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		return *this;
	}

	Vector2& Vector2::operator/=(float scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}

	Vector2& Vector2::operator/=(const Vector2& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
		return *this;
	}
}
