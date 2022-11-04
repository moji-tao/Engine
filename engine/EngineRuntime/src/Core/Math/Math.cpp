#include "EngineRuntime/include/Core/Math/Math.h"
#include "EngineRuntime/include/Core/Math/Angle.h"
#include "EngineRuntime/include/Core/Math/Matrix4x4.h"
#include "EngineRuntime/include/Core/Math/Matrix3x3.h"
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Quaternion.h"

namespace Engine
{
	Math::AngleUnit Math::k_AngleUnit = Math::AngleUnit::AU_DEGREE;

	float Math::Abs(float value)
	{
		return std::abs(value);
	}

	bool Math::IsNaN(float f)
	{
		return std::isnan(f);
	}

	float Math::Sqr(float value)
	{
		return value * value;
	}

	float Math::Sqrt(float value)
	{
		return std::sqrt(value);
	}

	float Math::InvSqrt(float value)
	{
		return 1.0f / std::sqrt(value);
	}

	bool Math::RealEqual(float a, float b, float tolerance)
	{
		return std::fabs(b - a) <= tolerance;
	}

	float Math::Clamp(float v, float min, float max)
	{
		return std::clamp(v, min, max);
	}

	float Math::GetMaxElement(float x, float y, float z)
	{
		return std::max({ x, y, z });
	}

	float Math::DegreesToRadians(float degrees)
	{
		return degrees * Math_fDeg2Rad;
	}

	float Math::RadiansToDegrees(float radians)
	{
		return radians * Math_fRad2Deg;
	}

	float Math::AngleUnitsToRadians(float angleunits)
	{
		if (k_AngleUnit == AngleUnit::AU_DEGREE)
			return angleunits * Math_fDeg2Rad;

		return angleunits;
	}

	float Math::RadiansToAngleUnits(float radians)
	{
		if (k_AngleUnit == AngleUnit::AU_DEGREE)
			return radians * Math_fRad2Deg;

		return radians;
	}

	float Math::AngleUnitsToDegrees(float angleunits)
	{
		if (k_AngleUnit == AngleUnit::AU_RADIAN)
			return angleunits * Math_fRad2Deg;

		return angleunits;
	}

	float Math::DegreesToAngleUnits(float degrees)
	{
		if (k_AngleUnit == AngleUnit::AU_RADIAN)
			return degrees * Math_fDeg2Rad;

		return degrees;
	}

	float Math::Sin(const Radian& rad)
	{
		return std::sin(rad.ValueRadians());
	}

	float Math::Sin(float value)
	{
		return std::sin(value);
	}

	float Math::Cos(const Radian& rad)
	{
		return std::cos(rad.ValueRadians());
	}

	float Math::Cos(float value)
	{
		return std::cos(value);
	}

	float Math::Tan(const Radian& rad)
	{
		return std::tan(rad.ValueRadians());
	}

	float Math::Tan(float value)
	{
		return std::tan(value);
	}

	Radian Math::Acos(float value)
	{
		if (-1.0 < value)
		{
			if (value < 1.0)
				return Radian(::acos(value));

			return Radian(0.0);
		}

		return Radian(Math_PI);
	}

	Radian Math::Asin(float value)
	{
		if (-1.0 < value)
		{
			if (value < 1.0)
				return Radian(::asin(value));

			return Radian(Math_HALF_PI);
		}

		return Radian(-Math_HALF_PI);
	}

	Radian Math::Atan(float value)
	{
		return Radian(std::atan(value));
	}

	Radian Math::Atan2(float y_v, float x_v)
	{
		return Radian(std::atan2(y_v, x_v));
	}

	Matrix4x4 Math::MakeViewMatrix(const Vector3& position, const Quaternion& orientation, const Matrix4x4* reflect_matrix)
	{
		Matrix4x4 viewMatrix;

		// View matrix is:
		//
		//  [ Lx  Uy  Dz  Tx  ]
		//  [ Lx  Uy  Dz  Ty  ]
		//  [ Lx  Uy  Dz  Tz  ]
		//  [ 0   0   0   1   ]
		//
		// Where T = -(Transposed(Rot) * Pos)

		// This is most efficiently done using 3x3 Matrices
		Matrix3x3 rot;
		orientation.GetRotationMatrix(rot);

		// Make the translation relative to new axes
		Matrix3x3 rotT = rot.Transpose();
		Vector3   trans = -rotT * position;

		// Make final matrix
		viewMatrix = Matrix4x4::IDENTITY;
		viewMatrix.SetData(rotT); // fills upper 3x3
		viewMatrix[0][3] = trans[0];
		viewMatrix[1][3] = trans[1];
		viewMatrix[2][3] = trans[2];

		// Deal with reflections
		if (reflect_matrix)
		{
			viewMatrix = viewMatrix * (*reflect_matrix);
		}

		return viewMatrix;
	}

	Matrix4x4 Math::MakeLookAtMatrix(const Vector3& eye_position, const Vector3& target_position, const Vector3& up_dir)
	{
		/*
		const Vector3& up = up_dir.GetNormalised();

		Vector3 w = (target_position - eye_position).GetNormalised();
		Vector3 s = w.CrossProduct(up).GetNormalised();
		Vector3 u = s.CrossProduct(w);

		Matrix4x4 view_mat = Matrix4x4::IDENTITY;

		view_mat[0][0] = s[0];
		view_mat[0][1] = s[1];
		view_mat[0][2] = s[2];
		view_mat[0][3] = -s.DotProduct(eye_position);
		view_mat[1][0] = u[0];
		view_mat[1][1] = u[1];
		view_mat[1][2] = u[2];
		view_mat[1][3] = -u.DotProduct(eye_position);
		view_mat[2][0] = w[0];
		view_mat[2][1] = w[1];
		view_mat[2][2] = w[2];
		view_mat[2][3] = w.DotProduct(eye_position);
		return view_mat;
		*/

		Vector3 w = (target_position - eye_position).GetNormalised();
		Vector3 u = up_dir.CrossProduct(w).GetNormalised();
		//Vector3 u = w.CrossProduct(up_dir).GetNormalised();
		Vector3 v = w.CrossProduct(u);
		//Vector3 v = u.CrossProduct(w);

		Matrix4x4 view_mat = Matrix4x4::IDENTITY;

		view_mat[0][0] = u[0];
		view_mat[0][1] = u[1];
		view_mat[0][2] = u[2];
		view_mat[0][3] = -eye_position.DotProduct(u);

		view_mat[1][0] = v[0];
		view_mat[1][1] = v[1];
		view_mat[1][2] = v[2];
		view_mat[1][3] = -eye_position.DotProduct(v);

		view_mat[2][0] = w[0];
		view_mat[2][1] = w[1];
		view_mat[2][2] = w[2];
		view_mat[2][3] = -eye_position.DotProduct(w);

		return view_mat;

	}

	Matrix4x4 Math::MakePerspectiveMatrix(Radian fovy, float aspect, float znear, float zfar)
	{
		/*
		float tan_half_fovy = Math::Tan(0.5f * fovy);

		Matrix4x4 ret = Matrix4x4::ZERO;
		ret[0][0] = 1.f / (aspect * tan_half_fovy);
		ret[1][1] = 1.f / tan_half_fovy;
		ret[2][2] = -zfar / (znear - zfar);
		ret[3][2] = -1.f;
		ret[2][3] = -(zfar * znear) / (zfar - znear);
		*/

		Matrix4x4 ret = Matrix4x4::ZERO;
		float tan_half_fovy = Math::Tan(0.5f * fovy);
		ret[0][0] = 1.0f / (aspect * tan_half_fovy);
		ret[1][1] = 1.0f / tan_half_fovy;
		ret[2][2] = zfar / (zfar - znear);
		ret[2][3] = -(znear * zfar) / (zfar - znear);
		ret[3][2] = 1.0f;

		return ret;
	}

	Matrix4x4 Math::MakeOrthographicProjectionMatrix(float left, float right, float bottom, float top, float znear, float zfar)
	{
		float inv_width = 1.0f / (right - left);
		float inv_height = 1.0f / (top - bottom);
		float inv_distance = 1.0f / (zfar - znear);

		float A = 2 * inv_width;
		float B = 2 * inv_height;
		float C = -(right + left) * inv_width;
		float D = -(top + bottom) * inv_height;
		float q = -2 * inv_distance;
		float qn = qn = -(zfar + znear) * inv_distance;

		// NB: This creates 'uniform' orthographic projection matrix,
		// which depth range [-1,1], right-handed rules
		//
		// [ A   0   0   C  ]
		// [ 0   B   0   D  ]
		// [ 0   0   q   qn ]
		// [ 0   0   0   1  ]
		//
		// A = 2 * / (right - left)
		// B = 2 * / (top - bottom)
		// C = - (right + left) / (right - left)
		// D = - (top + bottom) / (top - bottom)
		// q = - 2 / (far - near)
		// qn = - (far + near) / (far - near)

		Matrix4x4 proj_matrix = Matrix4x4::ZERO;
		proj_matrix[0][0] = A;
		proj_matrix[0][3] = C;
		proj_matrix[1][1] = B;
		proj_matrix[1][3] = D;
		proj_matrix[2][2] = q;
		proj_matrix[2][3] = qn;
		proj_matrix[3][3] = 1;

		return proj_matrix;
	}
}

