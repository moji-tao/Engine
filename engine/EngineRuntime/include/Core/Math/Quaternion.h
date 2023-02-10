#pragma once
#include "EngineRuntime/include/Core/Math/Matrix3x3.h"
#include "EngineRuntime/include/Core/Math/Matrix4x4.h"

namespace Engine
{
	class Quaternion
	{
	public:
		Quaternion() = default;

		Quaternion(float w_, float x_, float y_, float z_);

		Quaternion(Degree roll, Degree pitch, Degree yaw);

		Quaternion(Radian roll, Radian pitch, Radian yaw);

		explicit Quaternion(const Matrix3x3& rot);

		Quaternion(const Radian& angle, const Vector3& axis);

		Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis);

		const float* ptr() const;

		float* ptr();

	public:
		// 从旋转矩阵设置四元数
		void SetDataFromRotationMatrix(const Matrix3x3& rotation);

		// 四元数转换为旋转矩阵
		void GetRotationMatrix(Matrix3x3& rotation) const;

		// 四元数转换为旋转矩阵
		void GetRotationMatrix(Matrix4x4& rotation) const;

		// 给定旋转轴和旋转角设置四元数
		void SetDataFromAngleAxis(const Radian& angle, const Vector3& axis);

		// 给定旋转轴和旋转角获取四元数
		static Quaternion GetQuaternionFromAngleAxis(const Radian& angle, const Vector3& axis);

		void SetDataFromDirection(const Vector3& direction, const Vector3& up_direction);

		static Quaternion GetQuaternionFromDirection(const Vector3& direction, const Vector3& up_direction);

		void GetAngleAxis(Radian& angle, Vector3& axis) const;

		void SetDataFromAxis(const Vector3& x_axis, const Vector3& y_axis, const Vector3& z_axis);

		void GetAxis(Vector3& x_axis, Vector3& y_axis, Vector3& z_axis) const;

		Vector3 GetxAxis() const;

		Vector3 GetyAxis() const;

		Vector3 GetzAxis() const;

		bool IsNaN() const;

		float GetX() const;

		void SetX(float value);

		float GetY() const;

		void SetY(float value);

		float GetZ() const;

		void SetZ(float value);

		float GetW() const;

		void SetW(float value);

		float Dot(const Quaternion& rkQ) const;

		float Length() const;

		void Normalize();

		Quaternion Mul(const Quaternion& rhs);

		Quaternion Inverse() const;

		//Radian GetRoll(bool reproject_axis) const;

		Radian GetRoll() const; //

		//Radian GetPitch(bool reproject_axis) const;
		
		Radian GetPitch() const; //

		//Radian GetYaw(bool reproject_axis) const;

		Radian GetYaw() const; //

		static Quaternion sLerp(float t, const Quaternion& kp, const Quaternion& kq, bool shortest_path = false);

		static Quaternion nLerp(float t, const Quaternion& kp, const Quaternion& kq, bool shortest_path = false);

		Quaternion Conjugate() const;

	public:
		Quaternion operator+(const Quaternion& rhs) const;

		Quaternion& operator+=(const Quaternion& rhs);

		Quaternion operator-(const Quaternion& rhs) const;

		Quaternion& operator-=(const Quaternion& rhs);

		Quaternion operator*(const Quaternion& rhs) const;

		Quaternion& operator*=(const Quaternion& rhs);

		Quaternion operator*(float scalar) const;

		Quaternion& operator*=(float scalar);

		Vector3 operator*(const Vector3& rhs) const;

		Quaternion operator/(float scalar) const;

		Quaternion& operator/=(float scalar);

		friend Quaternion operator*(float scalar, const Quaternion& rhs);

		Quaternion operator-() const;

		bool operator==(const Quaternion& rhs) const;

		bool operator!=(const Quaternion& rhs) const;

	public:
		static const Quaternion ZERO;

		static const Quaternion IDENTITY;

		static const float k_epsilon;

	public:
		float x{ 0.f }, y{ 0.f }, z{ 0.f }, w{ 1.f };
	};
}
