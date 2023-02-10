#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Core/Math/Quaternion.h"
#include "EngineRuntime/include/Core/Math/Math.h"


namespace Engine
{
	const Quaternion Quaternion::ZERO(0, 0, 0, 0);

	const Quaternion Quaternion::IDENTITY(1, 0, 0, 0);

	const float Quaternion::k_epsilon = 1e-03f;

	Quaternion::Quaternion(float w_, float x_, float y_, float z_) : w(w_), x(x_), y(y_), z(z_) { }

	Quaternion::Quaternion(Degree roll, Degree pitch, Degree yaw)
	{
		/*
		w = Math::Cos(roll / 2) * Math::Cos(pitch / 2) * Math::Cos(yaw / 2) -
			Math::Sin(roll / 2) * Math::Sin(pitch / 2) * Math::Sin(yaw / 2);

		x = Math::Sin(roll / 2) * Math::Cos(pitch / 2) * Math::Cos(yaw / 2) +
			Math::Cos(roll / 2) * Math::Sin(pitch / 2) * Math::Sin(yaw / 2);

		y = Math::Cos(roll / 2) * Math::Sin(pitch / 2) * Math::Cos(yaw / 2) -
			Math::Sin(roll / 2) * Math::Cos(pitch / 2) * Math::Sin(yaw / 2);

		z = Math::Cos(roll / 2) * Math::Cos(pitch / 2) * Math::Sin(yaw / 2) +
			Math::Sin(roll / 2) * Math::Sin(pitch / 2) * Math::Cos(yaw / 2);
		*/
		roll /= 2;
		pitch /= 2;
		yaw /= 2;
		
		x = Math::Cos(yaw) * Math::Sin(pitch) * Math::Cos(roll) +
			Math::Sin(yaw) * Math::Cos(pitch) * Math::Sin(roll);
		y = Math::Sin(yaw) * Math::Cos(pitch) * Math::Cos(roll) -
			Math::Cos(yaw) * Math::Sin(pitch) * Math::Sin(roll);
		z = Math::Cos(yaw) * Math::Cos(pitch) * Math::Sin(roll) -
			Math::Sin(yaw) * Math::Sin(pitch) * Math::Cos(roll);
		w = Math::Cos(yaw) * Math::Cos(pitch) * Math::Cos(roll) +
			Math::Sin(yaw) * Math::Sin(pitch) * Math::Sin(roll);
	}

	Quaternion::Quaternion(Radian roll, Radian pitch, Radian yaw)
	{
		/*
		w = Math::Cos(roll / 2) * Math::Cos(pitch / 2) * Math::Cos(yaw / 2) -
			Math::Sin(roll / 2) * Math::Sin(pitch / 2) * Math::Sin(yaw / 2);

		x = Math::Sin(roll / 2) * Math::Cos(pitch / 2) * Math::Cos(yaw / 2) +
			Math::Cos(roll / 2) * Math::Sin(pitch / 2) * Math::Sin(yaw / 2);

		y = Math::Cos(roll / 2) * Math::Sin(pitch / 2) * Math::Cos(yaw / 2) -
			Math::Sin(roll / 2) * Math::Cos(pitch / 2) * Math::Sin(yaw / 2);

		z = Math::Cos(roll / 2) * Math::Cos(pitch / 2) * Math::Sin(yaw / 2) +
			Math::Sin(roll / 2) * Math::Sin(pitch / 2) * Math::Cos(yaw / 2);
		*/
		roll /= 2;
		pitch /= 2;
		yaw /= 2;
		x = Math::Cos(yaw) * Math::Sin(roll) * Math::Cos(pitch) +
			Math::Sin(yaw) * Math::Cos(roll) * Math::Sin(pitch);
		y = Math::Sin(yaw) * Math::Cos(roll) * Math::Cos(pitch) -
			Math::Cos(yaw) * Math::Sin(roll) * Math::Sin(pitch);
		z = Math::Cos(yaw) * Math::Cos(roll) * Math::Sin(pitch) -
			Math::Sin(yaw) * Math::Sin(roll) * Math::Cos(pitch);
		w = Math::Cos(yaw) * Math::Cos(roll) * Math::Cos(pitch) +
			Math::Sin(yaw) * Math::Sin(roll) * Math::Sin(pitch);

	}

	Quaternion::Quaternion(const Matrix3x3& rot)
	{
		SetDataFromRotationMatrix(rot);
	}

	Quaternion::Quaternion(const Radian& angle, const Vector3& axis)
	{
		SetDataFromAngleAxis(angle, axis);
	}

	Quaternion::Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
	{
		SetDataFromAxis(xaxis, yaxis, zaxis);
	}

	const float* Quaternion::ptr() const
	{
		return &w;
	}

	float* Quaternion::ptr()
	{
		return &w;
	}

	void Quaternion::SetDataFromRotationMatrix(const Matrix3x3& rotation)
	{
		/*
		float trace = rotation[0][0] + rotation[1][1] + rotation[2][2];
		float root;

		if (trace > 0.0)
		{
			// |w| > 1/2, may as well choose w > 1/2
			root = Math::Sqrt(trace + 1.0f); // 2w
			w = 0.5f * root;
			root = 0.5f / root; // 1/(4w)
			x = (rotation[2][1] - rotation[1][2]) * root;
			y = (rotation[0][2] - rotation[2][0]) * root;
			z = (rotation[1][0] - rotation[0][1]) * root;
		}
		else
		{
			// |w| <= 1/2
			size_t s_iNext[3] = { 1, 2, 0 };
			size_t i = 0;
			if (rotation[1][1] > rotation[0][0])
				i = 1;
			if (rotation[2][2] > rotation[i][i])
				i = 2;
			size_t j = s_iNext[i];
			size_t k = s_iNext[j];

			root = std::sqrt(rotation[i][i] - rotation[j][j] - rotation[k][k] + 1.0f);
			float* apkQuat[3] = { &x, &y, &z };
			*apkQuat[i] = 0.5f * root;
			root = 0.5f / root;
			w = (rotation[k][j] - rotation[j][k]) * root;
			*apkQuat[j] = (rotation[j][i] + rotation[i][j]) * root;
			*apkQuat[k] = (rotation[k][i] + rotation[i][k]) * root;
		}
		*/

		w = Math::Sqrt((rotation[0][0] + rotation[1][1] + rotation[2][2]) + 1) * 0.5f;

		if (w != 0.0f)
		{
			x = (rotation[1][2] - rotation[2][1]) / (4.0f * w);
			y = (rotation[2][0] - rotation[0][2]) / (4.0f * w);
			z = (rotation[0][1] - rotation[1][0]) / (4.0f * w);
		}
		else
		{
			if (rotation[0][0] >= rotation[1][1] && rotation[0][0] >= rotation[2][2])
			{
				x = Math::Sqrt(rotation[0][0] - rotation[1][1] - rotation[2][2] + 1) / 2;
				y = (rotation[0][1] + rotation[1][0]) / (4.0f * x);
				z = (rotation[0][2] + rotation[2][0]) / (4.0f * y);
				w = (rotation[1][2] - rotation[2][1]) / (4.0f * x);
			}
			else if (rotation[1][1] >= rotation[0][0] && rotation[1][1] >= rotation[2][2])
			{
				y = Math::Sqrt(rotation[1][1] - rotation[0][0] - rotation[2][2] + 1) / 2;
				x = (rotation[0][1] + rotation[1][0]) / (4.0f * y);
				z = (rotation[1][2] + rotation[2][1]) / (4.0f * y);
				w = (rotation[1][2] - rotation[2][1]) / (4.0f * x);
			}
			else
			{
				z = Math::Sqrt(rotation[2][2] - rotation[0][0] - rotation[1][1] + 1) / 2;
				y = (rotation[1][2] + rotation[2][1]) / (4.0f * z);
				x = (rotation[0][1] + rotation[1][0]) / (4.0f * y);
				w = (rotation[1][2] - rotation[2][1]) / (4.0f * x);
			}
		}
	}

	void Quaternion::GetRotationMatrix(Matrix3x3& rotation) const
	{
		/*
		float fTx = x + x;   // 2x
		float fTy = y + y;   // 2y
		float fTz = z + z;   // 2z
		float fTwx = fTx * w; // 2xw
		float fTwy = fTy * w; // 2yw
		float fTwz = fTz * w; // 2z2
		float fTxx = fTx * x; // 2x^2
		float fTxy = fTy * x; // 2xy
		float fTxz = fTz * x; // 2xz
		float fTyy = fTy * y; // 2y^2
		float fTyz = fTz * y; // 2yz
		float fTzz = fTz * z; // 2z^2

		rotation[0][0] = 1.0f - (fTyy + fTzz); // 1 - 2y^2 - 2z^2
		rotation[0][1] = fTxy - fTwz;          // 2xy - 2wz
		rotation[0][2] = fTxz + fTwy;          // 2xz + 2wy
		rotation[1][0] = fTxy + fTwz;          // 2xy + 2wz
		rotation[1][1] = 1.0f - (fTxx + fTzz); // 1 - 2x^2 - 2z^2
		rotation[1][2] = fTyz - fTwx;          // 2yz - 2wx
		rotation[2][0] = fTxz - fTwy;          // 2xz - 2wy
		rotation[2][1] = fTyz + fTwx;          // 2yz + 2wx
		rotation[2][2] = 1.0f - (fTxx + fTyy); // 1 - 2x^2 - 2y^2
		*/

		rotation[0][0] = 1.0f - 2 * y * y - 2 * z * z;		// 1 - 2y^2 - 2z^2
		rotation[0][1] = 2 * x * y + 2 * z * w;				// 2xy + 2zw
		rotation[0][2] = 2 * x * z + 2 * y * w;				// 2xz + 2yw
		rotation[1][0] = 2 * x * y - 2 * z * w;				// 2xy - 2zw
		rotation[1][1] = 1.0f - 2 * x * x - 2 * z * z;		// 1 - 2x^2 - 2z^2
		rotation[1][2] = 2 * y * z + 2 * x * w;				// 2yz + 2xw
		rotation[2][0] = 2 * x * z + 2 * y * w;				// 2xz + 2yw
		rotation[2][1] = 2 * y * z - 2 * x * w;				// 2yz - 2xw
		rotation[2][2] = 1.0f - 2 * x * x - 2 * y * y;		// 1 - 2x^2 - 2y^2

	}

	void Quaternion::GetRotationMatrix(Matrix4x4& rotation) const
	{
		/*
		float fTx = x + x;   // 2x
		float fTy = y + y;   // 2y
		float fTz = z + z;   // 2z
		float fTwx = fTx * w; // 2xw
		float fTwy = fTy * w; // 2yw
		float fTwz = fTz * w; // 2z2
		float fTxx = fTx * x; // 2x^2
		float fTxy = fTy * x; // 2xy
		float fTxz = fTz * x; // 2xz
		float fTyy = fTy * y; // 2y^2
		float fTyz = fTz * y; // 2yz
		float fTzz = fTz * z; // 2z^2

		rotation[0][0] = 1.0f - (fTyy + fTzz); // 1 - 2y^2 - 2z^2
		rotation[0][1] = fTxy - fTwz;          // 2xy - 2wz
		rotation[0][2] = fTxz + fTwy;          // 2xz + 2wy
		rotation[0][3] = 0;
		rotation[1][0] = fTxy + fTwz;          // 2xy + 2wz
		rotation[1][1] = 1.0f - (fTxx + fTzz); // 1 - 2x^2 - 2z^2
		rotation[1][2] = fTyz - fTwx;          // 2yz - 2wx
		rotation[1][3] = 0;
		rotation[2][0] = fTxz - fTwy;          // 2xz - 2wy
		rotation[2][1] = fTyz + fTwx;          // 2yz + 2wx
		rotation[2][2] = 1.0f - (fTxx + fTyy); // 1 - 2x^2 - 2y^2
		rotation[2][3] = 0;
		rotation[3][0] = 0;
		rotation[3][1] = 0;
		rotation[3][2] = 0;
		rotation[3][3] = 1;
		*/

		rotation[0][0] = 1.0f - 2 * y * y - 2 * z * z;		// 1 - 2y^2 - 2z^2
		rotation[0][1] = 2 * x * y + 2 * z * w;				// 2xy + 2zw
		rotation[0][2] = 2 * x * z + 2 * y * w;				// 2xz + 2yw
		rotation[0][3] = 0.0f;
		rotation[1][0] = 2 * x * y - 2 * z * w;				// 2xy - 2zw
		rotation[1][1] = 1.0f - 2 * x * x - 2 * z * z;		// 1 - 2x^2 - 2z^2
		rotation[1][2] = 2 * y * z + 2 * x * w;				// 2yz + 2xw
		rotation[1][3] = 0.0f;
		rotation[2][0] = 2 * x * z + 2 * y * w;				// 2xz + 2yw
		rotation[2][1] = 2 * y * z - 2 * x * w;				// 2yz - 2xw
		rotation[2][2] = 1.0f - 2 * x * x - 2 * y * y;		// 1 - 2x^2 - 2y^2
		rotation[2][3] = 0.0f;
		rotation[3][0] = 0.0f;
		rotation[3][1] = 0.0f;
		rotation[3][2] = 0.0f;
		rotation[3][3] = 1.0f;
	}

	void Quaternion::SetDataFromAngleAxis(const Radian& angle, const Vector3& axis)
	{
		// ASSERT:  axis[] is unit length
		//
		// The quaternion representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)
		Radian half_angle(0.5 * angle);
		float  sin_v = Math::Sin(half_angle);
		w = Math::Cos(half_angle);
		x = sin_v * axis.GetX();
		y = sin_v * axis.GetY();
		z = sin_v * axis.GetZ();
	}

	Quaternion Quaternion::GetQuaternionFromAngleAxis(const Radian& angle, const Vector3& axis)
	{
		Quaternion q;
		q.SetDataFromAngleAxis(angle, axis);
		return q;
	}

	void Quaternion::SetDataFromDirection(const Vector3& direction, const Vector3& up_direction)
	{
		Vector3 forward_direction = direction;
		//forward_direction.SetZ(0.0f);
		forward_direction.Normalize();

		Vector3 right_direction = forward_direction.CrossProduct(up_direction);

		SetDataFromAxis(right_direction, up_direction, forward_direction);
		Normalize();
	}

	Quaternion Quaternion::GetQuaternionFromDirection(const Vector3& direction, const Vector3& up_direction)
	{
		Quaternion object_orientation;
		object_orientation.SetDataFromDirection(direction, up_direction);
		return object_orientation;
	}

	void Quaternion::GetAngleAxis(Radian& angle, Vector3& axis) const
	{
		// (x,y,z,w) = (axis*sin(angle/2), cos(angle/2))

		float len = x * x + y * y + z * z;
		if (len != 0.0f)
		{
			angle = Math::Acos(w) * 2.0f;
			axis.x = x / Math::Sin(angle / 2);
			axis.y = y / Math::Sin(angle / 2);
			axis.z = z / Math::Sin(angle / 2);
		}
		else
		{
			angle = 0;
			axis.x = axis.y = 0;
			axis.z = 1;
		}
	}

	void Quaternion::SetDataFromAxis(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
	{
		Matrix3x3 rot;

		rot[0][0] = xaxis.GetX();
		rot[0][1] = xaxis.GetY();
		rot[0][2] = xaxis.GetZ();

		rot[1][0] = yaxis.GetX();
		rot[1][1] = yaxis.GetY();
		rot[1][2] = yaxis.GetZ();

		rot[2][0] = zaxis.GetX();
		rot[2][1] = zaxis.GetY();
		rot[2][2] = zaxis.GetZ();

		SetDataFromRotationMatrix(rot);
	}

	void Quaternion::GetAxis(Vector3& xaxis, Vector3& yaxis, Vector3& zaxis) const
	{
		Matrix3x3 rot;

		GetRotationMatrix(rot);

		xaxis.SetX(rot[0][0]);
		xaxis.SetY(rot[0][1]);
		xaxis.SetZ(rot[0][2]);

		yaxis.SetX(rot[1][0]);
		yaxis.SetY(rot[1][1]);
		yaxis.SetZ(rot[1][2]);

		zaxis.SetX(rot[2][0]);
		zaxis.SetY(rot[2][1]);
		zaxis.SetZ(rot[2][2]);
	}

	Vector3 Quaternion::GetxAxis() const
	{
		return Vector3(1.0f - 2 * y * y - 2 * z * z, 2 * x * y + 2 * z * w, 2 * x * z + 2 * y * w);
	}

	Vector3 Quaternion::GetyAxis() const
	{
		return Vector3(2 * x * y - 2 * z * w, 1.0f - 2 * x * x - 2 * z * z, 2 * y * z + 2 * x * w);
	}

	Vector3 Quaternion::GetzAxis() const
	{
		return Vector3(2 * x * z + 2 * y * w, 2 * y * z - 2 * x * w, 1.0f - 2 * x * x - 2 * y * y);
	}

	bool Quaternion::IsNaN() const
	{
		return Math::IsNaN(x) || Math::IsNaN(y) || Math::IsNaN(z) || Math::IsNaN(w);
	}

	float Quaternion::GetX() const
	{
		return x;
	}

	void Quaternion::SetX(float value)
	{
		x = value;
	}

	float Quaternion::GetY() const
	{
		return y;
	}

	void Quaternion::SetY(float value)
	{
		y = value;
	}

	float Quaternion::GetZ() const
	{
		return z;
	}

	void Quaternion::SetZ(float value)
	{
		z = value;
	}

	float Quaternion::GetW() const
	{
		return w;
	}

	void Quaternion::SetW(float value)
	{
		w = value;
	}

	float Quaternion::Dot(const Quaternion& rkQ) const
	{
		return w * rkQ.w + x * rkQ.x + y * rkQ.y + z * rkQ.z;
	}

	float Quaternion::Length() const
	{
		return Math::Sqrt(w * w + x * x + y * y + z * z);
	}

	void Quaternion::Normalize()
	{
		float factor = 1.0f / Length();
		*this = *this * factor;
	}

	Quaternion Quaternion::Mul(const Quaternion& rhs)
	{
		return (*this) * rhs;
	}

	Quaternion Quaternion::Inverse() const
	{
		float norm = w * w + x * x + y * y + z * z;
		if (norm > 0.0)
		{
			float inv_norm = 1.0f / norm;
			return Quaternion(w * inv_norm, -x * inv_norm, -y * inv_norm, -z * inv_norm);
		}
		else
		{
			// return an invalid result to flag the error
			return ZERO;
		}
	}

	/*
	Radian Quaternion::GetRoll(bool reproject_axis) const
	{
		if (reproject_axis)
		{
			// yaw = atan2(localz.x, localz.z)
			// pick parts of zAxis() implementation that we need
			float tx = 2.0f * x;
			float ty = 2.0f * y;
			float tz = 2.0f * z;
			float twy = ty * w;
			float txx = tx * x;
			float txz = tz * x;
			float tyy = ty * y;

			return Radian(Math::Atan2(txz + twy, 1.0f - (txx + tyy)));
		}
		else
		{
			// internal version
			//return Radian(Math::Asin(-2 * (x * z - w * y)));

			if (Math::Abs(2 * (x * z + w * y)) < 0.99999)
			{
				return Math::Atan2((w * x - y * z) * 2, 1 - (2 * (y * y + x * x)));
			}
			else
			{
				return Math::Atan2(2 * (y * z + w * x), 1 - (2 * (x * x + z * z)));
			}
		}
	}
	*/

	Radian Quaternion::GetRoll() const
	{
		return Math::Atan2(2 * (x * y + w * z), w * w - x * x + y * y - z * z);
	}

	/*
	Radian Quaternion::GetPitch(bool reproject_axis) const
	{
		if (reproject_axis)
		{
			// pitch = atan2(localy.z, localy.y)
			// pick parts of yAxis() implementation that we need
			float tx = 2.0f * x;
			//  float ty  = 2.0f*y;
			float tz = 2.0f * z;
			float twx = tx * w;
			float txx = tx * x;
			float tyz = tz * y;
			float tzz = tz * z;

			return Radian(Math::Atan2(tyz + twx, 1.0f - (txx + tzz)));
		}
		else
		{
			// internal version
			//return Radian(Math::Atan2(2 * (y * z + w * x), w * w - x * x - y * y + z * z));
			return Math::Asin(Math::Clamp(2 * (x * z + w * y), -1.0f, 1.0f));
		}
	}
	*/

	Radian Quaternion::GetPitch() const
	{
		return Math::Asin(-2 * (y * z - w * x));
	}

	/*
	Radian Quaternion::GetYaw(bool reproject_axis) const
	{
		if (reproject_axis)
		{
			// roll = atan2(localx.y, localx.x)
			// pick parts of xAxis() implementation that we need
			//  float tx  = 2.0*x;
			float ty = 2.0f * y;
			float tz = 2.0f * z;
			float twz = tz * w;
			float txy = ty * x;
			float tyy = ty * y;
			float tzz = tz * z;

			return Radian(Math::Atan2(txy + twz, 1.0f - (tyy + tzz)));
		}
		else
		{
			//return Radian(Math::Atan2(2 * (x * y + w * z), w * w + x * x - y * y - z * z));
			if (Math::Abs(2 * (x * z + w * y)) < 0.99999)
			{
				return Math::Atan2(2 * (w * z - x * y), 1 - (2 * (y * y + z * z)));
			}
			else
			{
				return Radian(0.0f);
			}
		}

	}
	*/

	Radian Quaternion::GetYaw() const
	{
		return Math::Atan2(2 * (x * z + w * y), w * w - x * x - y * y + z * z);
	}

	Quaternion Quaternion::sLerp(float t, const Quaternion& kp, const Quaternion& kq, bool shortest_path)
	{
		float cos_v = kp.Dot(kq);
		Quaternion kt;

		// Do we need to invert rotation?
		if (cos_v < 0.0f && shortest_path)
		{
			cos_v = -cos_v;
			kt = -kq;
		}
		else
		{
			kt = kq;
		}

		if (Math::Abs(cos_v) < 1 - k_epsilon)
		{
			// Standard case (slerp)
			float  sin_v = Math::Sqrt(1 - Math::Sqr(cos_v));
			Radian angle = Math::Atan2(sin_v, cos_v);
			float  inv_sin = 1.0f / sin_v;
			float  coeff0 = Math::Sin((1.0f - t) * angle) * inv_sin;
			float  coeff1 = Math::Sin(t * angle) * inv_sin;
			return coeff0 * kp + coeff1 * kt;
		}
		else
		{
			// There are two situations:
			// 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
			//    interpolation safely.
			// 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
			//    are an infinite number of possibilities interpolation. but we haven't
			//    have method to fix this case, so just use linear interpolation here.
			Quaternion r = (1.0f - t) * kp + t * kt;
			// taking the complement requires renormalization
			r.Normalize();
			return r;
		}
	}

	Quaternion Quaternion::nLerp(float t, const Quaternion& kp, const Quaternion& kq, bool shortest_path)
	{
		Quaternion result;
		float cos_value = kp.Dot(kq);
		if (cos_value < 0.0f && shortest_path)
		{
			result = kp + t * ((-kq) - kp);
		}
		else
		{
			result = kp + t * (kq - kp);
		}
		result.Normalize();
		return result;
	}

	Quaternion Quaternion::Conjugate() const
	{
		return Quaternion(w, -x, -y, -z);
	}

	Quaternion Quaternion::operator+(const Quaternion& rhs) const
	{
		return Quaternion(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Quaternion& Quaternion::operator+=(const Quaternion& rhs)
	{
		w += rhs.w;
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	Quaternion Quaternion::operator-(const Quaternion& rhs) const
	{
		return Quaternion(w - rhs.w, x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Quaternion& Quaternion::operator-=(const Quaternion& rhs)
	{
		w -= rhs.w;
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	Quaternion Quaternion::operator*(const Quaternion& rhs) const
	{
		return Quaternion(w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
			w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
			w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
			w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x);
	}

	Quaternion& Quaternion::operator*=(const Quaternion& rhs)
	{
		*this = *this * rhs;
		//w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;
		//x = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
		//y = w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z;
		//z = w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x;
		return *this;
	}

	Quaternion Quaternion::operator*(float scalar) const
	{
		return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
	}

	Quaternion& Quaternion::operator*=(float scalar)
	{
		w *= scalar;
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	Vector3 Quaternion::operator*(const Vector3& rhs) const
	{
		Quaternion result = *this * Quaternion(0, rhs.x, rhs.y, rhs.z) * (*this).Conjugate();
		return Vector3(result.x, result.y, result.z);
	}

	Quaternion Quaternion::operator/(float scalar) const
	{
		ASSERT(scalar != 0.0f);
		return Quaternion(w / scalar, x / scalar, y / scalar, z / scalar);
	}

	Quaternion& Quaternion::operator/=(float scalar)
	{
		ASSERT(scalar != 0.0f);
		w /= scalar;
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	Quaternion Quaternion::operator-() const
	{
		return Quaternion(-w, -x, -y, -z);
	}

	bool Quaternion::operator==(const Quaternion& rhs) const
	{
		return (rhs.x == x) && (rhs.y == y) && (rhs.z == z) && (rhs.w == w);
	}

	bool Quaternion::operator!=(const Quaternion& rhs) const
	{
		return (rhs.x != x) || (rhs.y != y) || (rhs.z != z) || (rhs.w != w);
	}

	Quaternion operator*(float scalar, const Quaternion& rhs)
	{
		return Quaternion(scalar * rhs.w, scalar * rhs.x, scalar * rhs.y, scalar * rhs.z);
	}
}
