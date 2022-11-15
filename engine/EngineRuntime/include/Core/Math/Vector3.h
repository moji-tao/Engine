#pragma once
namespace Engine
{
	class Radian;
	class Quaternion;

	class Vector2;

	class Vector3
	{
	public:
		Vector3();

		Vector3(float _x, float _y, float _z);

		explicit Vector3(const float coords[3]);

		explicit Vector3(const Vector2& vec);

		float* ptr();

		const float* ptr() const;

	public:
		float GetX() const;

		float GetY() const;

		float GetZ() const;

		void SetX(float scaler);

		void SetY(float scaler);

		void SetZ(float scaler);

		float Length() const;

		float SquaredLength() const;

		float Distance(const Vector3& rhs) const;

		float SquaredDistance(const Vector3& rhs) const;

		float DotProduct(const Vector3& vec) const;

		void Normalise();

		Vector3 CrossProduct(const Vector3& rhs) const;

		void MakeFloor(const Vector3& cmp);

		void MakeCeil(const Vector3& cmp);

		Radian GetAngle(const Vector3& dest) const;

		Quaternion GetRotationTo(const Vector3& dest, const Vector3& fallback_axis = Vector3::ZERO) const;

		bool IsZeroLength() const;

		bool IsZero() const;

		Vector3 GetNormalised() const;

		Vector3 Reflect(const Vector3& normal) const;

		Vector3 Project(const Vector3& normal) const;

		Vector3 GetAbsolute() const;

		static Vector3 Lerp(const Vector3& lhs, const Vector3& rhs, float alpha);

		static Vector3 Clamp(const Vector3& v, const Vector3& min, const Vector3& max);

		static float GetMaxElement(const Vector3& v);

		bool IsNaN() const;

	public:
		float operator[](size_t i) const;

		float& operator[](size_t i);

		bool operator==(const Vector3& rhs) const;

		bool operator!=(const Vector3& rhs) const;

		Vector3 operator+(const Vector3& rhs) const;

		Vector3 operator-(const Vector3& rhs) const;

		Vector3 operator*(float scalar) const;

		Vector3 operator*(const Vector3& rhs) const;

		Vector3 operator/(float scalar) const;

		Vector3 operator/(const Vector3& rhs) const;

		const Vector3& operator+() const;

		Vector3 operator-() const;

		friend Vector3 operator*(float scalar, const Vector3& rhs);

		friend Vector3 operator/(float scalar, const Vector3& rhs);

		friend Vector3 operator+(const Vector3& lhs, float rhs);

		friend Vector3 operator+(float lhs, const Vector3& rhs);

		friend Vector3 operator-(const Vector3& lhs, float rhs);

		friend Vector3 operator-(float lhs, const Vector3& rhs);

		Vector3& operator+=(const Vector3& rhs);

		Vector3& operator+=(float scalar);

		Vector3& operator-=(const Vector3& rhs);

		Vector3& operator-=(float scalar);

		Vector3& operator*=(float scalar);

		Vector3& operator*=(const Vector3& rhs);

		Vector3& operator/=(float scalar);

		Vector3& operator/=(const Vector3& rhs);

	public:
		//float m_Value[3];
		float x;
		float y;
		float z;

	public:
		static const Vector3 ZERO;
		static const Vector3 UNIT_X;
		static const Vector3 UNIT_Y;
		static const Vector3 UNIT_Z;
		static const Vector3 NEGATIVE_UNIT_X;
		static const Vector3 NEGATIVE_UNIT_Y;
		static const Vector3 NEGATIVE_UNIT_Z;
		static const Vector3 UNIT_SCALE;
	};
}
