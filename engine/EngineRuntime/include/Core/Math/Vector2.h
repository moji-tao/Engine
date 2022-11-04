#pragma once
namespace Engine
{
	class Vector2
	{
	public:
		Vector2();

		Vector2(float _x, float _y);

		explicit Vector2(float scaler);

		explicit Vector2(const float v[2]);

		float* ptr();

		const float* ptr() const;

	public:
		float GetX() const;

		float GetY() const;

		void SetX(float scaler);

		void SetY(float scaler);

		float Length() const;

		float SquaredLength() const;

		float Distance(const Vector2& rhs) const;

		float SquaredDistance(const Vector2& rhs) const;

		float DotProduct(const Vector2& vec) const;

		float Normalise();

		Vector2 MidPoint(const Vector2& vec) const;

		void MakeFloor(const Vector2& cmp);

		void MakeCeil(const Vector2& cmp);

		float crossProduct(const Vector2& rhs) const;

		bool IsZeroLength() const;

		Vector2 GetNormalised() const;

		Vector2 Reflect(const Vector2& normal) const;

		bool IsNaN() const;

		static Vector2 Lerp(const Vector2& lhs, const Vector2& rhs, float alpha);

	public:
		float operator[](size_t i) const;

		float& operator[](size_t i);

		bool operator==(const Vector2& rhs) const;

		bool operator!=(const Vector2& rhs) const;

		Vector2 operator+(const Vector2& rhs) const;

		Vector2 operator-(const Vector2& rhs) const;

		Vector2 operator*(float scaler) const;

		Vector2 operator*(const Vector2& rhs) const;

		Vector2 operator/(float scaler) const;

		Vector2 operator/(const Vector2& rhs) const;

		bool operator<(const Vector2& rhs) const;

		bool operator>(const Vector2& rhs) const;

		const Vector2& operator+() const;

		Vector2 operator-() const;

		friend Vector2 operator*(float scalar, const Vector2& rhs);

		friend Vector2 operator/(float scalar, const Vector2& rhs);

		friend Vector2 operator+(const Vector2& lhs, float rhs);

		friend Vector2 operator+(float lhs, const Vector2& rhs);

		friend Vector2 operator-(const Vector2& lhs, float rhs);

		friend Vector2 operator-(float lhs, const Vector2& rhs);

		Vector2& operator+=(const Vector2& rhs);

		Vector2& operator+=(float scalar);

		Vector2& operator-=(const Vector2& rhs);

		Vector2& operator-=(float scalar);

		Vector2& operator*=(float scalar);

		Vector2& operator*=(const Vector2& rhs);

		Vector2& operator/=(float scalar);

		Vector2& operator/=(const Vector2& rhs);

	private:
		float m_Value[2];

	public:
		static const Vector2 ZERO;
		static const Vector2 UNIT_X;
		static const Vector2 UNIT_Y;
		static const Vector2 NEGATIVE_UNIT_X;
		static const Vector2 NEGATIVE_UNIT_Y;
		static const Vector2 UNIT_SCALE;
	};
}
