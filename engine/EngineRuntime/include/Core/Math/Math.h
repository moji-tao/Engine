#pragma once
#include <cmath>
#include <algorithm>
#include <limits>
#include "EngineRuntime/include/Core/Math/Angle.h"
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Quaternion.h"
#include "EngineRuntime/include/Core/Math/Matrix4x4.h"

namespace Engine
{
	static const float Math_POS_INFINITY = std::numeric_limits<float>::infinity();
	static const float Math_NEG_INFINITY = -std::numeric_limits<float>::infinity();
	static const float Math_PI = 3.14159265358979323846264338327950288f;
	static const float Math_2PI = 3.14159265358979323846264338327950288f * Math_PI;
	static const float Math_ONE_OVER_PI = 1.0f / Math_PI;
	static const float Math_TWO_PI = 2.0f * Math_PI;
	static const float Math_HALF_PI = 0.5f * Math_PI;
	static const float Math_fDeg2Rad = Math_PI / 180.0f;
	static const float Math_fRad2Deg = 180.0f / Math_PI;
	static const float Math_LOG2 = log(2.0f);
	static const float Math_EPSILON = 1e-6f;

	static const float Float_EPSILON = FLT_EPSILON;
	static const float Double_EPSILON = DBL_EPSILON;

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

		//static float Sin(float value);

		static float Sin(const Degree& deg);

		static float Cos(const Radian& rad);

		//static float Cos(float value);

		static float Cos(const Degree& deg);

		static float Tan(const Radian& rad);

		//static float Tan(float value);

		static float Tan(const Degree& deg);

		static Radian Acos(float value);

		static Radian Asin(float value);

		static Radian Atan(float value);

		static Radian Atan2(float y_v, float x_v);

		static Matrix4x4 MakeOrthographicProjectionMatrix(float left, float right, float bottom, float top, float znear, float zfar);

		static Matrix4x4 MakeOrthographicProjectionMatrix(float wigth, float height, float znear, float zfar);

		static Matrix3x3 CreateRotationY(Radian radian);

		static Matrix4x4 MakeLookAtMatrix(const Vector3& eyeposition, const Vector3& target, const Vector3& up);

		static Matrix4x4 MakeLookAtMatrix(const Vector3& position, const Vector3& right, const Vector3& up, const Vector3& forward);

		static Matrix4x4 MakePerspectiveMatrix(Radian fovy, float aspect, float znear, float zfar);
		
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
