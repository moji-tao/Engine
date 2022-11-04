#pragma once
namespace Engine
{
	class Vector3;

	class Vector4
	{
	public:
		Vector4();

		Vector4(float x, float y, float z, float w);

		Vector4(const Vector3& v3, float w);

		explicit Vector4(float coords[4]);

		float* ptr();

		const float* ptr() const;

	public:
		float GetX() const;

		float GetY() const;

		float GetZ() const;

		float GetW() const;

		void SetX(float scaler);

		void SetY(float scaler);

		void SetZ(float scaler);

		void SetW(float scaler);

		float DotProduct(const Vector4& vec) const;

		bool IsNaN() const;

	public:
		float operator[](size_t i) const;

		float& operator[](size_t i);

		Vector4& operator=(float scalar);

		bool operator==(const Vector4& rhs) const;

		bool operator!=(const Vector4& rhs) const;

		Vector4 operator+(const Vector4& rhs) const;

		Vector4 operator-(const Vector4& rhs) const;

		Vector4 operator*(float scalar) const;

		Vector4 operator*(const Vector4& rhs) const;

		Vector4 operator/(float scalar) const;

		Vector4 operator/(const Vector4& rhs) const;

		const Vector4& operator+() const;

		Vector4 operator-() const;

		friend Vector4 operator*(float scalar, const Vector4& rhs);

		friend Vector4 operator/(float scalar, const Vector4& rhs);

		friend Vector4 operator+(const Vector4& lhs, float rhs);

		friend Vector4 operator+(float lhs, const Vector4& rhs);

		friend Vector4 operator-(const Vector4& lhs, float rhs);

		friend Vector4 operator-(float lhs, const Vector4& rhs);

		Vector4& operator+=(const Vector4& rhs);

		Vector4& operator-=(const Vector4& rhs);

		Vector4& operator*=(float scalar);

		Vector4& operator+=(float scalar);

		Vector4& operator-=(float scalar);

		Vector4& operator*=(const Vector4& rhs);

		Vector4& operator/=(float scalar);

		Vector4& operator/=(const Vector4& rhs);

	private:
		float m_Value[4];

	public:
		static const Vector4 ZERO;

		static const Vector4 UNIT_SCALE;
	};
}
