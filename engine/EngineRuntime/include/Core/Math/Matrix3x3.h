#pragma once

namespace Engine
{
	class Radian;
	class Angle;
	class Degree;

	class Vector2;
	class Vector3;
	class Vector4;
	class Matrix3x3;
	class Matrix4x4;
	class Quaternion;

	class Matrix3x3
	{
	public:
		Matrix3x3();

		explicit Matrix3x3(float arr[3][3]);

		Matrix3x3(float(&float_array)[9]);

		Matrix3x3(float entry00, float entry01, float entry02,
			float entry10, float entry11, float entry12,
			float entry20, float entry21, float entry22);

		Matrix3x3(const Vector3& row0, const Vector3& row1, const Vector3& row2);

		Matrix3x3(const Quaternion& q);

	public:
		void SetAllData(const float(&float_array)[9]);

		void GetAllData(float(&float_array)[9]) const;

		Vector3 GetColumn(size_t colIndex) const;

		void SetColumn(size_t colIndex, const Vector3& vec);

		void SetAllDataFromColumn(const Vector3& col0, const Vector3& col1, const Vector3& col2);

		Matrix3x3 Transpose() const;

		bool Inverse(Matrix3x3& invMat, float fTolerance = 1e-06) const;

		Matrix3x3 Inverse(float fTolerance = 1e-06) const;

		float Determinant() const;

		void CalculateQDUDecomposition(Matrix3x3& out_Q, Vector3& out_D, Vector3& out_U) const;

		void ToAngleAxis(Vector3& axis, Radian& angle) const;

		void ToAngleAxis(Vector3& axis, Degree& angle) const;

		void FromAngleAxis(const Vector3& axis, const Radian& radian);

		static Matrix3x3 Scale(const Vector3& scale);

	public:
		float* operator[](size_t rowIndex) const;

		bool operator==(const Matrix3x3& rhs) const;

		bool operator!=(const Matrix3x3& rhs) const;

		Matrix3x3 operator+(const Matrix3x3& rhs) const;

		Matrix3x3 operator-() const;

		Matrix3x3 operator-(const Matrix3x3& rhs) const;

		Matrix3x3 operator*(float scalar) const;

		Matrix3x3 operator*(const Matrix3x3& rhs) const;

		Vector3 operator*(const Vector3& rhs) const;

		friend Vector3 operator*(const Vector3& point, const Matrix3x3& rhs);

		friend Matrix3x3 operator*(float scalar, const Matrix3x3& rhs);

	public:
		static const Matrix3x3 ZERO;

		static const Matrix3x3 IDENTITY;

	private:
		float m_Value[3][3];
	};
}
