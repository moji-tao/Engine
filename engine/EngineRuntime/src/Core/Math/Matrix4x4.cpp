#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Core/Math/Matrix4x4.h"
#include "EngineRuntime/include/Core/Math/Quaternion.h"
#include "EngineRuntime/include/Core/Math/Math.h"

namespace Engine
{
	const Matrix4x4 Matrix4x4::ZERO(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	const Matrix4x4 Matrix4x4::ZEROAFFINE(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);

	const Matrix4x4 Matrix4x4::IDENTITY(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

	Matrix4x4::Matrix4x4()
	{
		operator=(IDENTITY);
	}

	Matrix4x4::Matrix4x4(const float(&float_array)[16])
	{
		memcpy(m_Value, float_array, 16 * sizeof(float));
	}

	Matrix4x4::Matrix4x4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		m_Value[0][0] = m00;
		m_Value[0][1] = m01;
		m_Value[0][2] = m02;
		m_Value[0][3] = m03;
		m_Value[1][0] = m10;
		m_Value[1][1] = m11;
		m_Value[1][2] = m12;
		m_Value[1][3] = m13;
		m_Value[2][0] = m20;
		m_Value[2][1] = m21;
		m_Value[2][2] = m22;
		m_Value[2][3] = m23;
		m_Value[3][0] = m30;
		m_Value[3][1] = m31;
		m_Value[3][2] = m32;
		m_Value[3][3] = m33;
	}

	Matrix4x4::Matrix4x4(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3)
	{
		m_Value[0][0] = row0.GetX();
		m_Value[0][1] = row0.GetY();
		m_Value[0][2] = row0.GetZ();
		m_Value[0][3] = row0.GetW();
		m_Value[1][0] = row1.GetX();
		m_Value[1][1] = row1.GetY();
		m_Value[1][2] = row1.GetZ();
		m_Value[1][3] = row1.GetW();
		m_Value[2][0] = row2.GetX();
		m_Value[2][1] = row2.GetY();
		m_Value[2][2] = row2.GetZ();
		m_Value[2][3] = row2.GetW();
		m_Value[3][0] = row3.GetX();
		m_Value[3][1] = row3.GetY();
		m_Value[3][2] = row3.GetZ();
		m_Value[3][3] = row3.GetW();
	}

	Matrix4x4::Matrix4x4(const Vector3& position, const Vector3& scale, const Quaternion& rotation)
	{
		MakeTransform(position, scale, rotation);
	}

	Matrix4x4::Matrix4x4(const Quaternion& rot)
	{
		Matrix3x3 m3x3;
		rot.GetRotationMatrix(m3x3);
		operator=(IDENTITY);
		SetData(m3x3);
	}

	float* Matrix4x4::Ptr()
	{
		return &m_Value[0][0];
	}

	const float* Matrix4x4::Ptr() const
	{
		return &m_Value[0][0];
	}

	void Matrix4x4::SetAllData(const float(&float_array)[16])
	{
		memcpy(m_Value, float_array, 16 * sizeof(float));
	}

	void Matrix4x4::GetAllData(float(&float_array)[16]) const
	{
		memcpy(float_array, m_Value, 16 * sizeof(float));
	}

	void Matrix4x4::SetData(const Matrix3x3& mat3)
	{
		m_Value[0][0] = mat3[0][0];
		m_Value[0][1] = mat3[0][1];
		m_Value[0][2] = mat3[0][2];
		m_Value[0][3] = 0;
		m_Value[1][0] = mat3[1][0];
		m_Value[1][1] = mat3[1][1];
		m_Value[1][2] = mat3[1][2];
		m_Value[1][3] = 0;
		m_Value[2][0] = mat3[2][0];
		m_Value[2][1] = mat3[2][1];
		m_Value[2][2] = mat3[2][2];
		m_Value[2][3] = 0;
		m_Value[3][0] = 0;
		m_Value[3][1] = 0;
		m_Value[3][2] = 0;
		m_Value[3][3] = 1;
	}

	Matrix4x4 Matrix4x4::Concatenate(const Matrix4x4& m2) const
	{
		return *this * m2;
	}

	Matrix4x4 Matrix4x4::Transpose() const
	{
		return Matrix4x4(
			m_Value[0][0],
			m_Value[1][0],
			m_Value[2][0],
			m_Value[3][0],
			m_Value[0][1],
			m_Value[1][1],
			m_Value[2][1],
			m_Value[3][1],
			m_Value[0][2],
			m_Value[1][2],
			m_Value[2][2],
			m_Value[3][2],
			m_Value[0][3],
			m_Value[1][3],
			m_Value[2][3],
			m_Value[3][3]);
	}

	float Matrix4x4::GetMinor(size_t r0, size_t r1, size_t r2, size_t c0, size_t c1, size_t c2) const
	{
		return m_Value[r0][c0] * (m_Value[r1][c1] * m_Value[r2][c2] - m_Value[r2][c1] * m_Value[r1][c2]) -
			m_Value[r0][c1] * (m_Value[r1][c0] * m_Value[r2][c2] - m_Value[r2][c0] * m_Value[r1][c2]) +
			m_Value[r0][c2] * (m_Value[r1][c0] * m_Value[r2][c1] - m_Value[r2][c0] * m_Value[r1][c1]);
	}

	void Matrix4x4::SetTrans(const Vector3& v)
	{
		m_Value[3][0] = v[0];
		m_Value[3][1] = v[1];
		m_Value[3][2] = v[2];
	}

	Vector3 Matrix4x4::GetTrans() const
	{
		return Vector3(m_Value[3][0], m_Value[3][1], m_Value[3][2]);
	}

	void Matrix4x4::MakeTrans(const Vector3& v)
	{
		m_Value[0][0] = 1.0f;
		m_Value[0][1] = 0.0f;
		m_Value[0][2] = 0.0f;
		m_Value[0][3] = 0.0f;
		m_Value[1][0] = 0.0f;
		m_Value[1][1] = 1.0f;
		m_Value[1][2] = 0.0f;
		m_Value[1][3] = 0.0f;
		m_Value[2][0] = 0.0f;
		m_Value[2][1] = 0.0f;
		m_Value[2][2] = 1.0f;
		m_Value[2][3] = 0.0f;
		m_Value[3][0] = v[0];
		m_Value[3][1] = v[1];
		m_Value[3][2] = v[2];
		m_Value[3][3] = 1.0f;
	}

	void Matrix4x4::MakeTrans(float tx, float ty, float tz)
	{
		m_Value[0][0] = 1.0f;
		m_Value[0][1] = 0.0f;
		m_Value[0][2] = 0.0f;
		m_Value[0][3] = 0.0f;
		m_Value[1][0] = 0.0f;
		m_Value[1][1] = 1.0f;
		m_Value[1][2] = 0.0f;
		m_Value[1][3] = 0.0f;
		m_Value[2][0] = 0.0f;
		m_Value[2][1] = 0.0f;
		m_Value[2][2] = 1.0f;
		m_Value[2][3] = 0.0f;
		m_Value[3][0] = tx;
		m_Value[3][1] = ty;
		m_Value[3][2] = tz;
		m_Value[3][3] = 1.0f;
	}

	Matrix4x4 Matrix4x4::GetTrans(const Vector3& v)
	{
		Matrix4x4 r = Matrix4x4::IDENTITY;

		r.m_Value[3][0] = v[0];
		r.m_Value[3][1] = v[1];
		r.m_Value[3][2] = v[2];

		return r;
	}

	Matrix4x4 Matrix4x4::GetTrans(float t_x, float t_y, float t_z)
	{
		Matrix4x4 r = Matrix4x4::IDENTITY;

		r.m_Value[3][0] = t_x;
		r.m_Value[3][1] = t_y;
		r.m_Value[3][2] = t_z;

		return r;
	}

	void Matrix4x4::SetScale(const Vector3& v)
	{
		m_Value[0][0] = v[0];
		m_Value[1][1] = v[1];
		m_Value[2][2] = v[2];
	}

	Matrix4x4 Matrix4x4::GetScale(const Vector3& v)
	{
		Matrix4x4 r = Matrix4x4::IDENTITY;

		r.m_Value[0][0] = v[0];
		r.m_Value[1][1] = v[1];
		r.m_Value[2][2] = v[2];

		return r;
	}

	Matrix4x4 Matrix4x4::BuildScaleMatrix(float s_x, float s_y, float s_z)
	{
		Matrix4x4 r = Matrix4x4::IDENTITY;

		r.m_Value[0][0] = s_x;
		r.m_Value[1][1] = s_y;
		r.m_Value[2][2] = s_z;

		return r;
	}

	void Matrix4x4::Extract3x3Matrix(Matrix3x3& m3x3) const
	{
		m3x3[0][0] = m_Value[0][0];
		m3x3[0][1] = m_Value[0][1];
		m3x3[0][2] = m_Value[0][2];
		m3x3[1][0] = m_Value[1][0];
		m3x3[1][1] = m_Value[1][1];
		m3x3[1][2] = m_Value[1][2];
		m3x3[2][0] = m_Value[2][0];
		m3x3[2][1] = m_Value[2][1];
		m3x3[2][2] = m_Value[2][2];
	}

	bool Matrix4x4::HasScale() const
	{
		// check magnitude of column vectors (==local axes)
		float t = m_Value[0][0] * m_Value[0][0] + m_Value[1][0] * m_Value[1][0] + m_Value[2][0] * m_Value[2][0];
		if (!Math::RealEqual(t, 1.0, (float)1e-04))
			return true;
		t = m_Value[0][1] * m_Value[0][1] + m_Value[1][1] * m_Value[1][1] + m_Value[2][1] * m_Value[2][1];
		if (!Math::RealEqual(t, 1.0, (float)1e-04))
			return true;
		t = m_Value[0][2] * m_Value[0][2] + m_Value[1][2] * m_Value[1][2] + m_Value[2][2] * m_Value[2][2];
		return !Math::RealEqual(t, 1.0, (float)1e-04);

	}

	bool Matrix4x4::HasNegativeScale() const
	{
		return Determinant() < 0;
	}

	Quaternion Matrix4x4::ExtractQuaternion() const
	{
		Matrix3x3 m3x3;
		Extract3x3Matrix(m3x3);
		return Quaternion(m3x3);
	}

	Matrix4x4 Matrix4x4::Adjoint() const
	{
		return Matrix4x4(
			GetMinor(1, 2, 3, 1, 2, 3),
			-GetMinor(0, 2, 3, 1, 2, 3),
			GetMinor(0, 1, 3, 1, 2, 3),
			-GetMinor(0, 1, 2, 1, 2, 3),

			-GetMinor(1, 2, 3, 0, 2, 3),
			GetMinor(0, 2, 3, 0, 2, 3),
			-GetMinor(0, 1, 3, 0, 2, 3),
			GetMinor(0, 1, 2, 0, 2, 3),

			GetMinor(1, 2, 3, 0, 1, 3),
			-GetMinor(0, 2, 3, 0, 1, 3),
			GetMinor(0, 1, 3, 0, 1, 3),
			-GetMinor(0, 1, 2, 0, 1, 3),

			-GetMinor(1, 2, 3, 0, 1, 2),
			GetMinor(0, 2, 3, 0, 1, 2),
			-GetMinor(0, 1, 3, 0, 1, 2),
			GetMinor(0, 1, 2, 0, 1, 2));
	}

	float Matrix4x4::Determinant() const
	{
		return m_Value[0][0] * GetMinor(1, 2, 3, 1, 2, 3) - m_Value[0][1] * GetMinor(1, 2, 3, 0, 2, 3) +
			m_Value[0][2] * GetMinor(1, 2, 3, 0, 1, 3) - m_Value[0][3] * GetMinor(1, 2, 3, 0, 1, 2);
	}

	void Matrix4x4::MakeTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation)
	{
		Matrix3x3 rot3x3;
		orientation.GetRotationMatrix(rot3x3);

		// Set up final matrix with scale, rotation and translation
		m_Value[0][0] = scale[0] * rot3x3[0][0];
		m_Value[0][1] = scale[1] * rot3x3[0][1];
		m_Value[0][2] = scale[1] * rot3x3[0][2];
		m_Value[0][3] = 0;

		m_Value[1][0] = scale[0] * rot3x3[1][0];
		m_Value[1][1] = scale[1] * rot3x3[1][1];
		m_Value[1][2] = scale[2] * rot3x3[1][2];
		m_Value[1][3] = 0;

		m_Value[2][0] = scale[0] * rot3x3[2][0];
		m_Value[2][1] = scale[1] * rot3x3[2][1];
		m_Value[2][2] = scale[2] * rot3x3[2][2];
		m_Value[2][3] = 0;

		// No projection term
		m_Value[3][0] = position[0];
		m_Value[3][1] = position[1];
		m_Value[3][2] = position[2];
		m_Value[3][3] = 1;
	}

	void Matrix4x4::MakeInverseTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation)
	{
		// Invert the parameters
		Vector3    inv_translate = -position;
		Vector3    inv_scale(1 / scale[0], 1 / scale[1], 1 / scale[2]);
		Quaternion inv_rot = orientation.Inverse();

		// Because we're inverting, order is translation, rotation, scale
		// So make translation relative to scale & rotation
		inv_translate = inv_rot * inv_translate; // rotate
		inv_translate *= inv_scale;              // scale

		// Next, make a 3x3 rotation matrix
		Matrix3x3 rot3x3;
		inv_rot.GetRotationMatrix(rot3x3);

		// Set up final matrix with scale, rotation and translation
		m_Value[0][0] = inv_scale[0] * rot3x3[0][0];
		m_Value[0][1] = inv_scale[1] * rot3x3[0][1];
		m_Value[0][2] = inv_scale[2] * rot3x3[0][2];
		m_Value[0][3] = 0;

		m_Value[1][0] = inv_scale[0] * rot3x3[1][0];
		m_Value[1][1] = inv_scale[1] * rot3x3[1][1];
		m_Value[1][2] = inv_scale[2] * rot3x3[1][2];
		m_Value[1][3] = 0;

		m_Value[2][0] = inv_scale[0] * rot3x3[2][0];
		m_Value[2][1] = inv_scale[1] * rot3x3[2][1];
		m_Value[2][2] = inv_scale[2] * rot3x3[2][2];
		m_Value[2][3] = 0;

		m_Value[3][0] = inv_translate[0];
		m_Value[3][1] = inv_translate[1];
		m_Value[3][2] = inv_translate[2];
		m_Value[3][3] = 1;
	}

	void Matrix4x4::Decomposition(Vector3& position, Vector3& scale, Quaternion& orientation) const
	{
		position = Vector3(m_Value[3][0], m_Value[3][1], m_Value[3][2]);

		Matrix3x3 m3x3;
		Extract3x3Matrix(m3x3);

		Matrix3x3 scale_2 = m3x3.Transpose() * m3x3;
		scale[0] = Math::Sqrt(scale_2[0][0]);
		scale[1] = Math::Sqrt(scale_2[1][1]);
		scale[2] = Math::Sqrt(scale_2[2][2]);

		orientation = Quaternion(m3x3 * scale_2);
		orientation.Normalize();
	}

	bool Matrix4x4::IsAffine(void) const
	{
		return m_Value[0][3] == 0 && m_Value[1][3] == 0 && m_Value[2][3] == 0 && m_Value[3][3] == 1;
	}

	Matrix4x4 Matrix4x4::Inverse() const
	{
		float m00 = m_Value[0][0], m01 = m_Value[0][1], m02 = m_Value[0][2], m03 = m_Value[0][3];
		float m10 = m_Value[1][0], m11 = m_Value[1][1], m12 = m_Value[1][2], m13 = m_Value[1][3];
		float m20 = m_Value[2][0], m21 = m_Value[2][1], m22 = m_Value[2][2], m23 = m_Value[2][3];
		float m30 = m_Value[3][0], m31 = m_Value[3][1], m32 = m_Value[3][2], m33 = m_Value[3][3];

		float v0 = m20 * m31 - m21 * m30;
		float v1 = m20 * m32 - m22 * m30;
		float v2 = m20 * m33 - m23 * m30;
		float v3 = m21 * m32 - m22 * m31;
		float v4 = m21 * m33 - m23 * m31;
		float v5 = m22 * m33 - m23 * m32;

		float t00 = +(v5 * m11 - v4 * m12 + v3 * m13);
		float t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
		float t20 = +(v4 * m10 - v2 * m11 + v0 * m13);
		float t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

		float invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

		float d00 = t00 * invDet;
		float d10 = t10 * invDet;
		float d20 = t20 * invDet;
		float d30 = t30 * invDet;

		float d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		float d11 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		float d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		float d31 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		v0 = m10 * m31 - m11 * m30;
		v1 = m10 * m32 - m12 * m30;
		v2 = m10 * m33 - m13 * m30;
		v3 = m11 * m32 - m12 * m31;
		v4 = m11 * m33 - m13 * m31;
		v5 = m12 * m33 - m13 * m32;

		float d02 = +(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		float d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		float d22 = +(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		float d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		v0 = m21 * m10 - m20 * m11;
		v1 = m22 * m10 - m20 * m12;
		v2 = m23 * m10 - m20 * m13;
		v3 = m22 * m11 - m21 * m12;
		v4 = m23 * m11 - m21 * m13;
		v5 = m23 * m12 - m22 * m13;

		float d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		float d13 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		float d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		float d33 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		return Matrix4x4(d00, d01, d02, d03, d10, d11, d12, d13, d20, d21, d22, d23, d30, d31, d32, d33);
	}

	/*
	Matrix4x4 Matrix4x4::InverseAffine() const
	{
		ASSERT(IsAffine());

		float m10 = m_Value[1][0], m11 = m_Value[1][1], m12 = m_Value[1][2];
		float m20 = m_Value[2][0], m21 = m_Value[2][1], m22 = m_Value[2][2];

		float t00 = m22 * m11 - m21 * m12;
		float t10 = m20 * m12 - m22 * m10;
		float t20 = m21 * m10 - m20 * m11;

		float m00 = m_Value[0][0], m01 = m_Value[0][1], m02 = m_Value[0][2];

		float inv_det = 1 / (m00 * t00 + m01 * t10 + m02 * t20);

		t00 *= inv_det;
		t10 *= inv_det;
		t20 *= inv_det;

		m00 *= inv_det;
		m01 *= inv_det;
		m02 *= inv_det;

		float r00 = t00;
		float r01 = m02 * m21 - m01 * m22;
		float r02 = m01 * m12 - m02 * m11;

		float r10 = t10;
		float r11 = m00 * m22 - m02 * m20;
		float r12 = m02 * m10 - m00 * m12;

		float r20 = t20;
		float r21 = m01 * m20 - m00 * m21;
		float r22 = m00 * m11 - m01 * m10;

		float m03 = m_Value[0][3], m13 = m_Value[1][3], m23 = m_Value[2][3];

		float r03 = -(r00 * m03 + r01 * m13 + r02 * m23);
		float r13 = -(r10 * m03 + r11 * m13 + r12 * m23);
		float r23 = -(r20 * m03 + r21 * m13 + r22 * m23);

		return Matrix4x4(r00, r01, r02, r03, r10, r11, r12, r13, r20, r21, r22, r23, 0, 0, 0, 1);
	}

	Matrix4x4 Matrix4x4::ConcatenateAffine(const Matrix4x4& m2) const
	{
		ASSERT(IsAffine() && m2.IsAffine());

		return Matrix4x4(
			m_Value[0][0] * m2.m_Value[0][0] + m_Value[0][1] * m2.m_Value[1][0] + m_Value[0][2] * m2.m_Value[2][0],
			m_Value[0][0] * m2.m_Value[0][1] + m_Value[0][1] * m2.m_Value[1][1] + m_Value[0][2] * m2.m_Value[2][1],
			m_Value[0][0] * m2.m_Value[0][2] + m_Value[0][1] * m2.m_Value[1][2] + m_Value[0][2] * m2.m_Value[2][2],
			m_Value[0][0] * m2.m_Value[0][3] + m_Value[0][1] * m2.m_Value[1][3] +
			m_Value[0][2] * m2.m_Value[2][3] + m_Value[0][3],

			m_Value[1][0] * m2.m_Value[0][0] + m_Value[1][1] * m2.m_Value[1][0] + m_Value[1][2] * m2.m_Value[2][0],
			m_Value[1][0] * m2.m_Value[0][1] + m_Value[1][1] * m2.m_Value[1][1] + m_Value[1][2] * m2.m_Value[2][1],
			m_Value[1][0] * m2.m_Value[0][2] + m_Value[1][1] * m2.m_Value[1][2] + m_Value[1][2] * m2.m_Value[2][2],
			m_Value[1][0] * m2.m_Value[0][3] + m_Value[1][1] * m2.m_Value[1][3] +
			m_Value[1][2] * m2.m_Value[2][3] + m_Value[1][3],

			m_Value[2][0] * m2.m_Value[0][0] + m_Value[2][1] * m2.m_Value[1][0] + m_Value[2][2] * m2.m_Value[2][0],
			m_Value[2][0] * m2.m_Value[0][1] + m_Value[2][1] * m2.m_Value[1][1] + m_Value[2][2] * m2.m_Value[2][1],
			m_Value[2][0] * m2.m_Value[0][2] + m_Value[2][1] * m2.m_Value[1][2] + m_Value[2][2] * m2.m_Value[2][2],
			m_Value[2][0] * m2.m_Value[0][3] + m_Value[2][1] * m2.m_Value[1][3] +
			m_Value[2][2] * m2.m_Value[2][3] + m_Value[2][3],
			0,
			0,
			0,
			1);
	}

	Vector3 Matrix4x4::TransformAffine(const Vector3& v) const
	{
		ASSERT(IsAffine());

		return Vector3(
			m_Value[0][0] * v[0] + m_Value[0][1] * v[1] + m_Value[0][2] * v[2] + m_Value[0][3],
			m_Value[1][0] * v[0] + m_Value[1][1] * v[1] + m_Value[1][2] * v[2] + m_Value[1][3],
			m_Value[2][0] * v[0] + m_Value[2][1] * v[1] + m_Value[2][2] * v[2] + m_Value[2][3]);
	}

	Vector4 Matrix4x4::TransformAffine(const Vector4& v) const
	{
		ASSERT(IsAffine());

		return Vector4(
			m_Value[0][0] * v[0] + m_Value[0][1] * v[1] + m_Value[0][2] * v[2] + m_Value[0][3] * v[3],
			m_Value[1][0] * v[0] + m_Value[1][1] * v[1] + m_Value[1][2] * v[2] + m_Value[1][3] * v[3],
			m_Value[2][0] * v[0] + m_Value[2][1] * v[1] + m_Value[2][2] * v[2] + m_Value[2][3] * v[3],
			v[3]);
	}

	Vector3 Matrix4x4::TransformCoord(const Vector3& v)
	{
		Vector4 temp(v, 1.0f);
		Vector4 ret = temp * (*this);
		if (ret[3] == 0.0f)
		{
			return Vector3::ZERO;
		}
		else
		{
			ret /= ret[3];
			return Vector3(ret[0], ret[1], ret[2]);
		}

		return Vector3::ZERO;
	}
	*/

	float* Matrix4x4::operator[](size_t row_index)
	{
		ASSERT(row_index < 4);
		return m_Value[row_index];
	}

	const float* Matrix4x4::operator[](size_t row_index) const
	{
		ASSERT(row_index < 4);
		return m_Value[row_index];
	}

	Matrix4x4 Matrix4x4::operator*(const Matrix4x4& m2) const
	{
		Matrix4x4 r;
		r.m_Value[0][0] = m_Value[0][0] * m2.m_Value[0][0] +
			m_Value[0][1] * m2.m_Value[1][0] +
			m_Value[0][2] * m2.m_Value[2][0] +
			m_Value[0][3] * m2.m_Value[3][0];

		r.m_Value[0][1] = m_Value[0][0] * m2.m_Value[0][1] +
			m_Value[0][1] * m2.m_Value[1][1] +
			m_Value[0][2] * m2.m_Value[2][1] +
			m_Value[0][3] * m2.m_Value[3][1];

		r.m_Value[0][2] = m_Value[0][0] * m2.m_Value[0][2] +
			m_Value[0][1] * m2.m_Value[1][2] +
			m_Value[0][2] * m2.m_Value[2][2] +
			m_Value[0][3] * m2.m_Value[3][2];

		r.m_Value[0][3] = m_Value[0][0] * m2.m_Value[0][3] +
			m_Value[0][1] * m2.m_Value[1][3] +
			m_Value[0][2] * m2.m_Value[2][3] +
			m_Value[0][3] * m2.m_Value[3][3];

		r.m_Value[1][0] = m_Value[1][0] * m2.m_Value[0][0] +
			m_Value[1][1] * m2.m_Value[1][0] +
			m_Value[1][2] * m2.m_Value[2][0] +
			m_Value[1][3] * m2.m_Value[3][0];

		r.m_Value[1][1] = m_Value[1][0] * m2.m_Value[0][1] +
			m_Value[1][1] * m2.m_Value[1][1] +
			m_Value[1][2] * m2.m_Value[2][1] +
			m_Value[1][3] * m2.m_Value[3][1];

		r.m_Value[1][2] = m_Value[1][0] * m2.m_Value[0][2] +
			m_Value[1][1] * m2.m_Value[1][2] +
			m_Value[1][2] * m2.m_Value[2][2] +
			m_Value[1][3] * m2.m_Value[3][2];

		r.m_Value[1][3] = m_Value[1][0] * m2.m_Value[0][3] +
			m_Value[1][1] * m2.m_Value[1][3] +
			m_Value[1][2] * m2.m_Value[2][3] +
			m_Value[1][3] * m2.m_Value[3][3];

		r.m_Value[2][0] = m_Value[2][0] * m2.m_Value[0][0] +
			m_Value[2][1] * m2.m_Value[1][0] +
			m_Value[2][2] * m2.m_Value[2][0] +
			m_Value[2][3] * m2.m_Value[3][0];

		r.m_Value[2][1] = m_Value[2][0] * m2.m_Value[0][1] +
			m_Value[2][1] * m2.m_Value[1][1] +
			m_Value[2][2] * m2.m_Value[2][1] +
			m_Value[2][3] * m2.m_Value[3][1];

		r.m_Value[2][2] = m_Value[2][0] * m2.m_Value[0][2] +
			m_Value[2][1] * m2.m_Value[1][2] +
			m_Value[2][2] * m2.m_Value[2][2] +
			m_Value[2][3] * m2.m_Value[3][2];

		r.m_Value[2][3] = m_Value[2][0] * m2.m_Value[0][3] +
			m_Value[2][1] * m2.m_Value[1][3] +
			m_Value[2][2] * m2.m_Value[2][3] +
			m_Value[2][3] * m2.m_Value[3][3];

		r.m_Value[3][0] = m_Value[3][0] * m2.m_Value[0][0] +
			m_Value[3][1] * m2.m_Value[1][0] +
			m_Value[3][2] * m2.m_Value[2][0] +
			m_Value[3][3] * m2.m_Value[3][0];

		r.m_Value[3][1] = m_Value[3][0] * m2.m_Value[0][1] +
			m_Value[3][1] * m2.m_Value[1][1] +
			m_Value[3][2] * m2.m_Value[2][1] +
			m_Value[3][3] * m2.m_Value[3][1];

		r.m_Value[3][2] = m_Value[3][0] * m2.m_Value[0][2] +
			m_Value[3][1] * m2.m_Value[1][2] +
			m_Value[3][2] * m2.m_Value[2][2] +
			m_Value[3][3] * m2.m_Value[3][2];

		r.m_Value[3][3] = m_Value[3][0] * m2.m_Value[0][3] +
			m_Value[3][1] * m2.m_Value[1][3] +
			m_Value[3][2] * m2.m_Value[2][3] +
			m_Value[3][3] * m2.m_Value[3][3];

		return r;
	}

	Vector3 Matrix4x4::operator*(const Vector3& v) const
	{
		Vector3 r;

		float inv_w = 1.0f / (m_Value[3][0] * v[0] + m_Value[3][1] * v[1] + m_Value[3][2] * v[2] + m_Value[3][3]);

		r[0] = (m_Value[0][0] * v[0] + m_Value[0][1] * v[1] + m_Value[0][2] * v[2] + m_Value[0][3]) * inv_w;
		r[1] = (m_Value[1][0] * v[0] + m_Value[1][1] * v[1] + m_Value[1][2] * v[2] + m_Value[1][3]) * inv_w;
		r[2] = (m_Value[2][0] * v[0] + m_Value[2][1] * v[1] + m_Value[2][2] * v[2] + m_Value[2][3]) * inv_w;

		return r;
	}
	/*
	Vector4 Matrix4x4::operator*(const Vector4& v) const
	{
		return Vector4(
			m_Value[0][0] * v[0] + m_Value[0][1] * v[1] + m_Value[0][2] * v[2] + m_Value[0][3] * v[3],
			m_Value[1][0] * v[0] + m_Value[1][1] * v[1] + m_Value[1][2] * v[2] + m_Value[1][3] * v[3],
			m_Value[2][0] * v[0] + m_Value[2][1] * v[1] + m_Value[2][2] * v[2] + m_Value[2][3] * v[3],
			m_Value[3][0] * v[0] + m_Value[3][1] * v[1] + m_Value[3][2] * v[2] + m_Value[3][3] * v[3]);
	}
	*/
	Matrix4x4 Matrix4x4::operator*(float scalar) const
	{
		return Matrix4x4(
			scalar * m_Value[0][0],
			scalar * m_Value[0][1],
			scalar * m_Value[0][2],
			scalar * m_Value[0][3],
			scalar * m_Value[1][0],
			scalar * m_Value[1][1],
			scalar * m_Value[1][2],
			scalar * m_Value[1][3],
			scalar * m_Value[2][0],
			scalar * m_Value[2][1],
			scalar * m_Value[2][2],
			scalar * m_Value[2][3],
			scalar * m_Value[3][0],
			scalar * m_Value[3][1],
			scalar * m_Value[3][2],
			scalar * m_Value[3][3]);
	}

	Vector4 operator*(const Vector4& v, const Matrix4x4& mat)
	{
		return Vector4(
			v[0] * mat[0][0] + v[1] * mat[1][0] + v[2] * mat[2][0] + v[3] * mat[3][0],
			v[0] * mat[0][1] + v[1] * mat[1][1] + v[2] * mat[2][1] + v[3] * mat[3][1],
			v[0] * mat[0][2] + v[1] * mat[1][2] + v[2] * mat[2][2] + v[3] * mat[3][2],
			v[0] * mat[0][3] + v[1] * mat[1][3] + v[2] * mat[2][3] + v[3] * mat[3][3]);
	}

	Matrix4x4 Matrix4x4::operator+(const Matrix4x4& m2) const
	{
		Matrix4x4 r;

		r.m_Value[0][0] = m_Value[0][0] + m2.m_Value[0][0];
		r.m_Value[0][1] = m_Value[0][1] + m2.m_Value[0][1];
		r.m_Value[0][2] = m_Value[0][2] + m2.m_Value[0][2];
		r.m_Value[0][3] = m_Value[0][3] + m2.m_Value[0][3];

		r.m_Value[1][0] = m_Value[1][0] + m2.m_Value[1][0];
		r.m_Value[1][1] = m_Value[1][1] + m2.m_Value[1][1];
		r.m_Value[1][2] = m_Value[1][2] + m2.m_Value[1][2];
		r.m_Value[1][3] = m_Value[1][3] + m2.m_Value[1][3];

		r.m_Value[2][0] = m_Value[2][0] + m2.m_Value[2][0];
		r.m_Value[2][1] = m_Value[2][1] + m2.m_Value[2][1];
		r.m_Value[2][2] = m_Value[2][2] + m2.m_Value[2][2];
		r.m_Value[2][3] = m_Value[2][3] + m2.m_Value[2][3];

		r.m_Value[3][0] = m_Value[3][0] + m2.m_Value[3][0];
		r.m_Value[3][1] = m_Value[3][1] + m2.m_Value[3][1];
		r.m_Value[3][2] = m_Value[3][2] + m2.m_Value[3][2];
		r.m_Value[3][3] = m_Value[3][3] + m2.m_Value[3][3];

		return r;
	}

	Matrix4x4 Matrix4x4::operator-(const Matrix4x4& m2) const
	{
		Matrix4x4 r;
		r.m_Value[0][0] = m_Value[0][0] - m2.m_Value[0][0];
		r.m_Value[0][1] = m_Value[0][1] - m2.m_Value[0][1];
		r.m_Value[0][2] = m_Value[0][2] - m2.m_Value[0][2];
		r.m_Value[0][3] = m_Value[0][3] - m2.m_Value[0][3];

		r.m_Value[1][0] = m_Value[1][0] - m2.m_Value[1][0];
		r.m_Value[1][1] = m_Value[1][1] - m2.m_Value[1][1];
		r.m_Value[1][2] = m_Value[1][2] - m2.m_Value[1][2];
		r.m_Value[1][3] = m_Value[1][3] - m2.m_Value[1][3];

		r.m_Value[2][0] = m_Value[2][0] - m2.m_Value[2][0];
		r.m_Value[2][1] = m_Value[2][1] - m2.m_Value[2][1];
		r.m_Value[2][2] = m_Value[2][2] - m2.m_Value[2][2];
		r.m_Value[2][3] = m_Value[2][3] - m2.m_Value[2][3];

		r.m_Value[3][0] = m_Value[3][0] - m2.m_Value[3][0];
		r.m_Value[3][1] = m_Value[3][1] - m2.m_Value[3][1];
		r.m_Value[3][2] = m_Value[3][2] - m2.m_Value[3][2];
		r.m_Value[3][3] = m_Value[3][3] - m2.m_Value[3][3];

		return r;
	}

	bool Matrix4x4::operator==(const Matrix4x4& m2) const
	{
		return !(m_Value[0][0] != m2.m_Value[0][0] || m_Value[0][1] != m2.m_Value[0][1] || m_Value[0][2] != m2.m_Value[0][2] ||
			m_Value[0][3] != m2.m_Value[0][3] || m_Value[1][0] != m2.m_Value[1][0] || m_Value[1][1] != m2.m_Value[1][1] ||
			m_Value[1][2] != m2.m_Value[1][2] || m_Value[1][3] != m2.m_Value[1][3] || m_Value[2][0] != m2.m_Value[2][0] ||
			m_Value[2][1] != m2.m_Value[2][1] || m_Value[2][2] != m2.m_Value[2][2] || m_Value[2][3] != m2.m_Value[2][3] ||
			m_Value[3][0] != m2.m_Value[3][0] || m_Value[3][1] != m2.m_Value[3][1] || m_Value[3][2] != m2.m_Value[3][2] ||
			m_Value[3][3] != m2.m_Value[3][3]);
	}

	bool Matrix4x4::operator!=(const Matrix4x4& m2) const
	{
		return m_Value[0][0] != m2.m_Value[0][0] || m_Value[0][1] != m2.m_Value[0][1] || m_Value[0][2] != m2.m_Value[0][2] ||
			m_Value[0][3] != m2.m_Value[0][3] || m_Value[1][0] != m2.m_Value[1][0] || m_Value[1][1] != m2.m_Value[1][1] ||
			m_Value[1][2] != m2.m_Value[1][2] || m_Value[1][3] != m2.m_Value[1][3] || m_Value[2][0] != m2.m_Value[2][0] ||
			m_Value[2][1] != m2.m_Value[2][1] || m_Value[2][2] != m2.m_Value[2][2] || m_Value[2][3] != m2.m_Value[2][3] ||
			m_Value[3][0] != m2.m_Value[3][0] || m_Value[3][1] != m2.m_Value[3][1] || m_Value[3][2] != m2.m_Value[3][2] ||
			m_Value[3][3] != m2.m_Value[3][3];
	}
}
