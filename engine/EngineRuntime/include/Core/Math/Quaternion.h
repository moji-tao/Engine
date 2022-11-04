#pragma once
namespace Engine
{
	class Matrix3x3;
	class Radian;
	class Vector3;
	class Matrix4x4;

	class Quaternion
	{
	public:
		Quaternion() = default;

		Quaternion(float w_, float x_, float y_, float z_);

		explicit Quaternion(const Matrix3x3& rot);

		Quaternion(const Radian& angle, const Vector3& axis);

		Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis);

		const float* ptr() const;

		float* ptr();

	public:
		void SetDataFromRotationMatrix(const Matrix3x3& rotation);

		void GetRotationMatrix(Matrix3x3& rotation) const;

		void GetRotationMatrix(Matrix4x4& rotation) const;

		void SetDataFromAngleAxis(const Radian& angle, const Vector3& axis);

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

		void Normalise();

		Quaternion Mul(const Quaternion& rhs);

		Quaternion Inverse() const;

		Radian GetRoll(bool reproject_axis = true) const;

		Radian GetPitch(bool reproject_axis = true) const;

		Radian GetYaw(bool reproject_axis = true) const;

		static Quaternion sLerp(float t, const Quaternion& kp, const Quaternion& kq, bool shortest_path = false);

		static Quaternion nLerp(float t, const Quaternion& kp, const Quaternion& kq, bool shortest_path = false);

		Quaternion Conjugate() const;

	public:
		Quaternion operator+(const Quaternion& rhs) const;

		Quaternion operator-(const Quaternion& rhs) const;

		Quaternion operator*(const Quaternion& rhs) const;

		Quaternion operator*(float scalar) const;

		Vector3 operator*(const Vector3& rhs) const;

		Quaternion operator/(float scalar) const;

		friend Quaternion operator*(float scalar, const Quaternion& rhs);

		Quaternion operator-() const;

		bool operator==(const Quaternion& rhs) const;

		bool operator!=(const Quaternion& rhs) const;

	public:
		static const Quaternion ZERO;

		static const Quaternion IDENTITY;

		static const float k_epsilon;

	private:
		float w{ 1.f }, x{ 0.f }, y{ 0.f }, z{ 0.f };
	};
}
