#include "EngineRuntime/include/Core/Math/Math.h"
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

	/*
	float Math::Sin(float value)
	{
		return std::sin(value);
	}
	*/

	float Math::Sin(const Degree& deg)
	{
		return std::sin(DegreesToRadians(deg.ValueDegrees()));
	}

	float Math::Cos(const Radian& rad)
	{
		return std::cos(rad.ValueRadians());
	}

	/*
	float Math::Cos(float value)
	{
		return std::cos(value);
	}
	*/

	float Math::Cos(const Degree& deg)
	{
		return std::cos(DegreesToRadians(deg.ValueDegrees()));
	}

	float Math::Tan(const Radian& rad)
	{
		return std::tan(rad.ValueRadians());
	}

	/*
	float Math::Tan(float value)
	{
		return std::tan(value);
	}
	*/

	float Math::Tan(const Degree& deg)
	{
		return std::tan(DegreesToRadians(deg.ValueDegrees()));
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

	Matrix4x4 Math::MakeOrthographicProjectionMatrix(float left, float right, float bottom, float top, float znear, float zfar)
	{
		float reciprocalWidth = 1.0f / (right - left);
		float reciprocalHeight = 1.0f / (top - bottom);
		float fRange = 1.0f / (zfar - znear);

		Matrix4x4 M = Matrix4x4::IDENTITY;

		M[0][0] = reciprocalWidth + reciprocalWidth;

		M[1][1] = reciprocalHeight + reciprocalHeight;

		M[2][2] = fRange;

		M[3][0] = -(left + right) * reciprocalWidth;
		M[3][1] = -(top + bottom) * reciprocalHeight;
		M[3][2] = -fRange * znear;
		M[3][3] = 1.0f;

		return M;
	}

	Matrix4x4 Math::MakeOrthographicProjectionMatrix(float wigth, float height, float znear, float zfar)
	{
		Matrix4x4 M = Matrix4x4::IDENTITY;

		float fRange = 1.0f / (zfar - znear);

		M[0][0] = 2.0f / wigth;

		M[1][1] = 2.0f / height;

		M[2][2] = fRange;

		M[3][2] = -fRange * znear;

		return M;
	}

	Matrix3x3 Math::CreateRotationY(Radian radian)
	{
		Matrix3x3 mat(Matrix3x3::IDENTITY);

		mat[0][0] = Cos(radian);
		mat[0][2] = -Sin(radian);
		mat[2][0] = Sin(radian);
		mat[2][2] = Cos(radian);

		return mat;
	}

	Matrix4x4 Math::MakeLookAtMatrix(const Vector3& eyeposition, const Vector3& target, const Vector3& up)
	{
		/*
		Vector3 Look = target - eyeposition;
		Look.Normalize();
		Vector3 Right = Look.CrossProduct(up);
		Vector3 Up = Look.CrossProduct(Right);
		Up.Normalize();
		Right = Up.CrossProduct(Look);

		// Fill in the view matrix entries.
		float x = -eyeposition.DotProduct(Right);
		float y = -eyeposition.DotProduct(Up);
		float z = -eyeposition.DotProduct(Look);

		Matrix4x4 View;

		View[0][0] = Right.x;
		View[1][0] = Right.y;
		View[2][0] = Right.z;
		View[3][0] = x;

		View[0][1] = Up.x;
		View[1][1] = Up.y;
		View[2][1] = Up.z;
		View[3][1] = y;

		View[0][2] = Look.x;
		View[1][2] = Look.y;
		View[2][2] = Look.z;
		View[3][2] = z;

		View[0][3] = 0.0f;
		View[1][3] = 0.0f;
		View[2][3] = 0.0f;
		View[3][3] = 1.0f;

		return View;
		*/
		Vector3 R2 = target - eyeposition;
		R2.Normalize();
		Vector3 R0 = up.CrossProduct(R2);
		if (R0.IsZero())
		{
			return Matrix4x4::IDENTITY;
		}
		R0.Normalize();
		Vector3 R1 = R2.CrossProduct(R0);

		Vector3 negEyePosition = -eyeposition;

		// Fill in the view matrix entries.
		float d0 = R0.DotProduct(negEyePosition);
		float d1 = R1.DotProduct(negEyePosition);
		float d2 = R2.DotProduct(negEyePosition);

		Matrix4x4 View;

		View[0][0] = R0.x;
		View[1][0] = R0.y;
		View[2][0] = R0.z;
		View[3][0] = d0;

		View[0][1] = R1.x;
		View[1][1] = R1.y;
		View[2][1] = R1.z;
		View[3][1] = d1;

		View[0][2] = R2.x;
		View[1][2] = R2.y;
		View[2][2] = R2.z;
		View[3][2] = d2;

		View[0][3] = 0.0f;
		View[1][3] = 0.0f;
		View[2][3] = 0.0f;
		View[3][3] = 1.0f;

		return View;
	}

	Matrix4x4 Math::MakeLookAtMatrix(const Vector3& position, const Vector3& right, const Vector3& up, const Vector3& forward)
	{
		Matrix4x4 View;
		Vector3 Look = forward;
		Vector3 Right = right;
		Vector3 Up = up;

		// Keep camera's axes orthogonal to each other and of unit length.
		Look.Normalize();
		Up = Look.CrossProduct(Right);
		Up.Normalize();
		
		// Up, Look already ortho-normal, so no need to normalize cross product.
		Right = Up.CrossProduct(Look);

		// Fill in the view matrix entries.
		float x = -position.DotProduct(Right);
		float y = -position.DotProduct(Up);
		float z = -position.DotProduct(Look);

		View[0][0] = Right.x;
		View[1][0] = Right.y;
		View[2][0] = Right.z;
		View[3][0] = x;

		View[0][1] = Up.x;
		View[1][1] = Up.y;
		View[2][1] = Up.z;
		View[3][1] = y;

		View[0][2] = Look.x;
		View[1][2] = Look.y;
		View[2][2] = Look.z;
		View[3][2] = z;

		View[0][3] = 0.0f;
		View[1][3] = 0.0f;
		View[2][3] = 0.0f;
		View[3][3] = 1.0f;

		return View;
	}

	Matrix4x4 Math::MakePerspectiveMatrix(Radian fovy, float aspect, float znear, float zfar)
	{
		float Height = Cos(fovy / 2) / Sin(fovy / 2);
		float Width = Height / aspect;
		float fRange = zfar / (zfar - znear);

		Matrix4x4 M;
		M[0][0] = Width;
		M[0][1] = 0.0f;
		M[0][2] = 0.0f;
		M[0][3] = 0.0f;

		M[1][0] = 0.0f;
		M[1][1] = Height;
		M[1][2] = 0.0f;
		M[1][3] = 0.0f;

		M[2][0] = 0.0f;
		M[2][1] = 0.0f;
		M[2][2] = fRange;
		M[2][3] = 1.0f;

		M[3][0] = 0.0f;
		M[3][1] = 0.0f;
		M[3][2] = -fRange * znear;
		M[3][3] = 0.0f;

		return M;
	}
}

