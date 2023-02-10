#include <cstring>
#include <cmath>
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Core/Math/Matrix3x3.h"
#include "EngineRuntime/include/Core/Math/Math.h"

namespace Engine
{
	const Matrix3x3 Matrix3x3::ZERO(0, 0, 0, 0, 0, 0, 0, 0, 0);

	const Matrix3x3 Matrix3x3::IDENTITY(1, 0, 0, 0, 1, 0, 0, 0, 1);

	Matrix3x3::Matrix3x3()
	{
		operator=(IDENTITY);
	}

	Matrix3x3::Matrix3x3(float arr[3][3])
	{
		memcpy(m_Value, arr, 9 * sizeof(float));
	}

	Matrix3x3::Matrix3x3(float(&float_array)[9])
	{
		memcpy(m_Value, float_array, 9 * sizeof(float));
	}

	Matrix3x3::Matrix3x3(float entry00, float entry01, float entry02, float entry10, float entry11, float entry12, float entry20, float entry21, float entry22)
	{
		m_Value[0][0] = entry00;
		m_Value[0][1] = entry01;
		m_Value[0][2] = entry02;
		m_Value[1][0] = entry10;
		m_Value[1][1] = entry11;
		m_Value[1][2] = entry12;
		m_Value[2][0] = entry20;
		m_Value[2][1] = entry21;
		m_Value[2][2] = entry22;
	}

	Matrix3x3::Matrix3x3(const Vector3& row0, const Vector3& row1, const Vector3& row2)
	{
		m_Value[0][0] = row0[0];
		m_Value[0][1] = row0[1];
		m_Value[0][2] = row0[2];
		m_Value[1][0] = row1[0];
		m_Value[1][1] = row1[1];
		m_Value[1][2] = row1[2];
		m_Value[2][0] = row2[0];
		m_Value[2][1] = row2[1];
		m_Value[2][2] = row2[2];
	}

	Matrix3x3::Matrix3x3(const Quaternion& q)
	{
		q.GetRotationMatrix(*this);
	}

	void Matrix3x3::SetAllData(const float(&float_array)[9])
	{
		memcpy(m_Value, float_array, 9 * sizeof(float));
	}

	void Matrix3x3::GetAllData(float(&float_array)[9]) const
	{
		memcpy(float_array, m_Value, 9 * sizeof(float));
	}

	Vector3 Matrix3x3::GetColumn(size_t colIndex) const
	{
		ASSERT(0 <= colIndex && colIndex < 3);
		return Vector3(m_Value[0][colIndex], m_Value[1][colIndex], m_Value[2][colIndex]);
	}

	void Matrix3x3::SetColumn(size_t colIndex, const Vector3& vec)
	{
		m_Value[0][colIndex] = vec[0];
		m_Value[1][colIndex] = vec[1];
		m_Value[2][colIndex] = vec[2];
	}

	void Matrix3x3::SetAllDataFromColumn(const Vector3& col0, const Vector3& col1, const Vector3& col2)
	{
		SetColumn(0, col0);
		SetColumn(1, col1);
		SetColumn(2, col2);
	}

	Matrix3x3 Matrix3x3::Transpose() const
	{
		Matrix3x3 transpose_v;
		for (size_t row_index = 0; row_index < 3; row_index++)
		{
			for (size_t col_index = 0; col_index < 3; col_index++)
				transpose_v[row_index][col_index] = m_Value[col_index][row_index];
		}
		return transpose_v;
	}

	bool Matrix3x3::Inverse(Matrix3x3& invMat, float fTolerance) const
	{
		// Invert a 3x3 using cofactors.  This is about 8 times faster than
		// the Numerical Recipes code which uses Gaussian elimination.

		float det = Determinant();
		if (std::fabs(det) <= fTolerance)
			return false;

		invMat[0][0] = m_Value[1][1] * m_Value[2][2] - m_Value[1][2] * m_Value[2][1];
		invMat[0][1] = m_Value[0][2] * m_Value[2][1] - m_Value[0][1] * m_Value[2][2];
		invMat[0][2] = m_Value[0][1] * m_Value[1][2] - m_Value[0][2] * m_Value[1][1];
		invMat[1][0] = m_Value[1][2] * m_Value[2][0] - m_Value[1][0] * m_Value[2][2];
		invMat[1][1] = m_Value[0][0] * m_Value[2][2] - m_Value[0][2] * m_Value[2][0];
		invMat[1][2] = m_Value[0][2] * m_Value[1][0] - m_Value[0][0] * m_Value[1][2];
		invMat[2][0] = m_Value[1][0] * m_Value[2][1] - m_Value[1][1] * m_Value[2][0];
		invMat[2][1] = m_Value[0][1] * m_Value[2][0] - m_Value[0][0] * m_Value[2][1];
		invMat[2][2] = m_Value[0][0] * m_Value[1][1] - m_Value[0][1] * m_Value[1][0];

		float inv_det = 1.0f / det;
		for (size_t row_index = 0; row_index < 3; row_index++)
		{
			for (size_t col_index = 0; col_index < 3; col_index++)
				invMat[row_index][col_index] *= inv_det;
		}

		return true;
	}

	Matrix3x3 Matrix3x3::Inverse(float fTolerance) const
	{
		Matrix3x3 inv = ZERO;
		Inverse(inv, fTolerance);
		return inv;
	}

	float Matrix3x3::Determinant() const
	{
		float cofactor00 = m_Value[1][1] * m_Value[2][2] - m_Value[1][2] * m_Value[2][1];
		float cofactor10 = m_Value[1][2] * m_Value[2][0] - m_Value[1][0] * m_Value[2][2];
		float cofactor20 = m_Value[1][0] * m_Value[2][1] - m_Value[1][1] * m_Value[2][0];

		float det = m_Value[0][0] * cofactor00 + m_Value[0][1] * cofactor10 + m_Value[0][2] * cofactor20;

		return det;
	}
	
	void Matrix3x3::ToAngleAxis(Vector3& axis, Radian& radian) const
	{
		// Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
		// The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
		// I is the identity and
		//
		//       +-        -+
		//   P = |  0 -z +y |
		//       | +z  0 -x |
		//       | -y +x  0 |
		//       +-        -+
		//
		// If A > 0, R represents a counterclockwise rotation about the axis in
		// the sense of looking from the tip of the axis vector towards the
		// origin.  Some algebra will show that
		//
		//   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
		//
		// In the event that A = pi, R-R^t = 0 which prevents us from extracting
		// the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
		// P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
		// z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
		// it does not matter which sign you choose on the square roots.

		float trace = m_Value[0][0] + m_Value[1][1] + m_Value[2][2];
		float cos_v = 0.5f * (trace - 1.0f);
		radian = Math::Acos(cos_v); // in [0,PI]

		if (radian > Radian(0.0))
		{
			if (radian < Radian(Math_PI))
			{
				axis[0] = m_Value[2][1] - m_Value[1][2];
				axis[1] = m_Value[0][2] - m_Value[2][0];
				axis[2] = m_Value[1][0] - m_Value[0][1];
				axis.Normalize();
			}
			else
			{
				// angle is PI
				float half_inv;
				if (m_Value[0][0] >= m_Value[1][1])
				{
					// r00 >= r11
					if (m_Value[0][0] >= m_Value[2][2])
					{
						// r00 is maximum diagonal term
						axis[0] = 0.5f * Math::Sqrt(m_Value[0][0] - m_Value[1][1] - m_Value[2][2] + 1.0f);
						half_inv = 0.5f / axis[0];
						axis[1] = half_inv * m_Value[0][1];
						axis[2] = half_inv * m_Value[0][2];
					}
					else
					{
						// r22 is maximum diagonal term
						axis[2] = 0.5f * Math::Sqrt(m_Value[2][2] - m_Value[0][0] - m_Value[1][1] + 1.0f);
						half_inv = 0.5f / axis[2];
						axis[0] = half_inv * m_Value[0][2];
						axis[1] = half_inv * m_Value[1][2];
					}
				}
				else
				{
					// r11 > r00
					if (m_Value[1][1] >= m_Value[2][2])
					{
						// r11 is maximum diagonal term
						axis[1] = 0.5f * Math::Sqrt(m_Value[1][1] - m_Value[0][0] - m_Value[2][2] + 1.0f);
						half_inv = 0.5f / axis[1];
						axis[0] = half_inv * m_Value[0][1];
						axis[2] = half_inv * m_Value[1][2];
					}
					else
					{
						// r22 is maximum diagonal term
						axis[2] = 0.5f * Math::Sqrt(m_Value[2][2] - m_Value[0][0] - m_Value[1][1] + 1.0f);
						half_inv = 0.5f / axis[2];
						axis[0] = half_inv * m_Value[0][2];
						axis[1] = half_inv * m_Value[1][2];
					}
				}
			}
		}
		else
		{
			// The angle is 0 and the matrix is the identity.  Any axis will
			// work, so just use the x-axis.
			axis[0] = 1.0;
			axis[1] = 0.0;
			axis[2] = 0.0;
		}
	}
	
	void Matrix3x3::ToAngleAxis(Vector3& axis, Degree& angle) const
	{
		Radian r;
		ToAngleAxis(axis, r);
		angle = r;

	}

	void Matrix3x3::FromAngleAxis(const Vector3& axis, const Radian& radian)
	{
		float cos_v = Math::Cos(radian);
		float sin_v = Math::Sin(radian);
		float one_minus_cos = 1.0f - cos_v;
		float x2 = axis[0] * axis[0];
		float y2 = axis[1] * axis[1];
		float z2 = axis[2] * axis[2];
		float xym = axis[0] * axis[1] * one_minus_cos;
		float xzm = axis[0] * axis[2] * one_minus_cos;
		float yzm = axis[1] * axis[2] * one_minus_cos;
		float x_sin_v = axis[0] * sin_v;
		float y_sin_v = axis[1] * sin_v;
		float z_sinv = axis[2] * sin_v;

		m_Value[0][0] = x2 * one_minus_cos + cos_v;
		m_Value[0][1] = xym + z_sinv;
		m_Value[0][2] = xzm + y_sin_v;
		m_Value[1][0] = xym - z_sinv;
		m_Value[1][1] = y2 * one_minus_cos + cos_v;
		m_Value[1][2] = yzm + x_sin_v;
		m_Value[2][0] = xzm - y_sin_v;
		m_Value[2][1] = yzm - x_sin_v;
		m_Value[2][2] = z2 * one_minus_cos + cos_v;

	}

	Matrix3x3 Matrix3x3::Scale(const Vector3& scale)
	{
		Matrix3x3 mat = ZERO;

		mat.m_Value[0][0] = scale[0];
		mat.m_Value[1][1] = scale[1];
		mat.m_Value[2][2] = scale[2];

		return mat;
	}

	float* Matrix3x3::operator[](size_t rowIndex) const
	{
		ASSERT(rowIndex < 3);
		return (float*)m_Value[rowIndex];
	}

	bool Matrix3x3::operator==(const Matrix3x3& rhs) const
	{
		for (size_t row_index = 0; row_index < 3; row_index++)
		{
			for (size_t col_index = 0; col_index < 3; col_index++)
			{
				if (m_Value[row_index][col_index] != rhs.m_Value[row_index][col_index])
					return false;
			}
		}
		return true;
	}

	bool Matrix3x3::operator!=(const Matrix3x3& rhs) const
	{
		return !operator==(rhs);
	}

	Matrix3x3 Matrix3x3::operator+(const Matrix3x3& rhs) const
	{
		Matrix3x3 sum;
		for (size_t row_index = 0; row_index < 3; row_index++)
		{
			for (size_t col_index = 0; col_index < 3; col_index++)
			{
				sum.m_Value[row_index][col_index] = m_Value[row_index][col_index] + rhs.m_Value[row_index][col_index];
			}
		}
		return sum;
	}

	Matrix3x3 Matrix3x3::operator-() const
	{
		Matrix3x3 neg;
		for (size_t row_index = 0; row_index < 3; row_index++)
		{
			for (size_t col_index = 0; col_index < 3; col_index++)
				neg[row_index][col_index] = -m_Value[row_index][col_index];
		}
		return neg;
	}

	Matrix3x3 Matrix3x3::operator-(const Matrix3x3& rhs) const
	{
		Matrix3x3 sum;
		for (size_t row_index = 0; row_index < 3; row_index++)
		{
			for (size_t col_index = 0; col_index < 3; col_index++)
			{
				sum.m_Value[row_index][col_index] = m_Value[row_index][col_index] - rhs.m_Value[row_index][col_index];
			}
		}
		return sum;
	}

	Matrix3x3 Matrix3x3::operator*(float scalar) const
	{
		Matrix3x3 sum;
		for (size_t row_index = 0; row_index < 3; row_index++)
		{
			for (size_t col_index = 0; col_index < 3; col_index++)
			{
				sum.m_Value[row_index][col_index] = m_Value[row_index][col_index] * scalar;
			}
		}
		return sum;
	}

	Matrix3x3 Matrix3x3::operator*(const Matrix3x3& rhs) const
	{
		Matrix3x3 prod;
		for (size_t row_index = 0; row_index < 3; row_index++)
		{
			for (size_t col_index = 0; col_index < 3; col_index++)
			{
				prod.m_Value[row_index][col_index] = m_Value[row_index][0] * rhs.m_Value[0][col_index] +
					m_Value[row_index][1] * rhs.m_Value[1][col_index] +
					m_Value[row_index][2] * rhs.m_Value[2][col_index];
			}
		}
		return prod;
	}
	/*
	Vector3 Matrix3x3::operator*(const Vector3& rhs) const
	{
		Vector3 prod;
		for (size_t row_index = 0; row_index < 3; row_index++)
		{
			prod[row_index] =
				m_Value[row_index][0] * rhs[0] + m_Value[row_index][1] * rhs[1] + m_Value[row_index][2] * rhs[2];
		}
		return prod;
	}
	*/
	Vector3 operator*(const Vector3& point, const Matrix3x3& rhs)
	{
		return Vector3(
			point[0] * rhs[0][0] + point[1] * rhs[1][0] + point[2] * rhs[2][0],
			point[0] * rhs[0][1] + point[1] * rhs[1][1] + point[2] * rhs[2][1],
			point[0] * rhs[0][2] + point[1] * rhs[1][2] + point[2] * rhs[2][2]
		);
	}

	Matrix3x3 operator*(float scalar, const Matrix3x3& rhs)
	{
		Matrix3x3 prod;
		for (size_t row_index = 0; row_index < 3; row_index++)
		{
			for (size_t col_index = 0; col_index < 3; col_index++)
			{
				prod[row_index][col_index] = scalar * rhs.m_Value[row_index][col_index];
			}
		}
		return prod;
	}
}
