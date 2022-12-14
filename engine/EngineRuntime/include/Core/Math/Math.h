#pragma once
#include <cmath>
#include <algorithm>
#include <limits>

namespace Engine
{
	static const float Math_POS_INFINITY = std::numeric_limits<float>::infinity();
	static const float Math_NEG_INFINITY = -std::numeric_limits<float>::infinity();
	static const float Math_PI = 3.14159265358979323846264338327950288f;
	static const float Math_ONE_OVER_PI = 1.0f / Math_PI;
	static const float Math_TWO_PI = 2.0f * Math_PI;
	static const float Math_HALF_PI = 0.5f * Math_PI;
	static const float Math_fDeg2Rad = Math_PI / 180.0f;
	static const float Math_fRad2Deg = 180.0f / Math_PI;
	static const float Math_LOG2 = log(2.0f);
	static const float Math_EPSILON = 1e-6f;

	static const float Float_EPSILON = FLT_EPSILON;
	static const float Double_EPSILON = DBL_EPSILON;

	class Radian;
	class Angle;
	class Degree;

	class Vector2;
	class Vector3;
	class Vector4;
	class Matrix3x3;
	class Matrix4x4;
	class Quaternion;

	class Math
	{
	public:
		static float Abs(float value);

		static bool IsNaN(float f);

		static float Sqr(float value);

		static float Sqrt(float value);

		static float InvSqrt(float value);

		static bool RealEqual(float a, float b, float tolerance = std::numeric_limits<float>::epsilon());

		static float Clamp(float v, float min, float max);

		static float GetMaxElement(float x, float y, float z);

		static float DegreesToRadians(float degrees);

		static float RadiansToDegrees(float radians);

		static float AngleUnitsToRadians(float angleunits);

		static float RadiansToAngleUnits(float radians);

		static float AngleUnitsToDegrees(float angleunits);

		static float DegreesToAngleUnits(float degrees);

		static float Sin(const Radian& rad);

		static float Sin(float value);

		static float Cos(const Radian& rad);

		static float Cos(float value);

		static float Tan(const Radian& rad);

		static float Tan(float value);

		static Radian Acos(float value);

		static Radian Asin(float value);

		static Radian Atan(float value);

		static Radian Atan2(float y_v, float x_v);

		static Matrix4x4 MakeViewMatrix(const Vector3& position, const Quaternion& orientation, const Matrix4x4* reflect_matrix = nullptr);

		static Matrix4x4 MakeLookAtMatrix(const Vector3& eye_position, const Vector3& target_position, const Vector3& up_dir);

		static Matrix4x4 MakePerspectiveMatrix(Radian fovy, float aspect, float znear, float zfar);

		static Matrix4x4 MakeOrthographicProjectionMatrix(float left, float right, float bottom, float top, float znear, float zfar);

		template<class T>
		static constexpr T Max(const T& A, const T& B)
		{
			return std::max(A, B);
		}

		template<class T>
		static constexpr T Min(const T& A, const T& B)
		{
			return std::min(A, B);
		}

		template<class T>
		static constexpr T Max3(const T& A, const T& B, const T& C)
		{
			//return std::max({ A, B, C });
			return std::max(std::max(A, B), C);
		}

		template<class T>
		static constexpr T Min3(const T& A, const T& B, const T& C)
		{
			//return std::min({ A, B, C });
			return std::min(std::min(A, B), C);
		}

		template<class T>
		static constexpr T Lerp(const T& A, const T& B, const float t)
		{
			return A + (B - A) * t;
		}

	private:
		enum class AngleUnit
		{
			AU_DEGREE,
			AU_RADIAN
		};

		static AngleUnit k_AngleUnit;
	};
}
