#pragma once
#include <cstdint>
#include "EngineRuntime/include/Core/Math/Vector4.h"
#include "EngineRuntime/include/Core/Math/Matrix3x3.h"

namespace Engine
{
	class Matrix4x4
	{
	public:
		Matrix4x4();

		Matrix4x4(const float(&float_array)[16]);

		Matrix4x4(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);

		Matrix4x4(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3);

		Matrix4x4(const Vector3& position, const Vector3& scale, const Quaternion& rotation);

		Matrix4x4(const Quaternion& rot);

	public:
		float* Ptr();

		const float* Ptr() const;

		void SetAllData(const float(&float_array)[16]);

		void GetAllData(float(&float_array)[16]) const;

		void SetData(const Matrix3x3& mat3);

		Matrix4x4 Concatenate(const Matrix4x4& m2) const;

		Matrix4x4 Transpose() const;

		float GetMinor(size_t r0, size_t r1, size_t r2, size_t c0, size_t c1, size_t c2) const;

		void SetTrans(const Vector3& v);

		Vector3 GetTrans() const;

		void MakeTrans(const Vector3& v); //

		void MakeTrans(float tx, float ty, float tz); //

		static Matrix4x4 GetTrans(const Vector3& v);

		static Matrix4x4 GetTrans(float t_x, float t_y, float t_z);

		void SetScale(const Vector3& v);

		static Matrix4x4 GetScale(const Vector3& v);

		static Matrix4x4 BuildScaleMatrix(float s_x, float s_y, float s_z);

		void Extract3x3Matrix(Matrix3x3& m3x3) const;

		bool HasScale() const;

		bool HasNegativeScale() const;

		Quaternion ExtractQuaternion() const;

		Matrix4x4 Adjoint() const;

		float Determinant() const;

		void MakeTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation);

		void MakeInverseTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation);

		void Decomposition(Vector3& position, Vector3& scale, Quaternion& orientation) const;

		bool IsAffine(void) const;

		Matrix4x4 Inverse() const;

		/*
		Matrix4x4 InverseAffine() const;

		Matrix4x4 ConcatenateAffine(const Matrix4x4& m2) const;

		Vector3 TransformAffine(const Vector3& v) const;

		Vector4 TransformAffine(const Vector4& v) const;

		Matrix4x4 Inverse() const;

		Vector3 TransformCoord(const Vector3& v);
		*/

	public:
		float* operator[](size_t row_index);

		const float* operator[](size_t row_index) const;

		Matrix4x4 operator*(const Matrix4x4& m2) const;

		Vector3 operator*(const Vector3& v) const;

		//Vector4 operator*(const Vector4& v) const;

		Matrix4x4 operator*(float scalar) const;

		friend Vector4 operator*(const Vector4& v, const Matrix4x4& mat);

		Matrix4x4 operator+(const Matrix4x4& m2) const;

		Matrix4x4 operator-(const Matrix4x4& m2) const;

		bool operator==(const Matrix4x4& m2) const;

		bool operator!=(const Matrix4x4& m2) const;

	public:
		static const Matrix4x4 ZERO;

		static const Matrix4x4 ZEROAFFINE;

		static const Matrix4x4 IDENTITY;

	private:
		float m_Value[4][4];
	};
}
