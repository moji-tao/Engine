#include "EngineRuntime/include/Function/Render/RenderCamera.h"
#include "EngineRuntime/include/Core/Math/Math.h"

namespace Engine
{
	void RenderCamera::SetCurrentCameraType(RenderCameraType type)
	{
		mCurrentCameraType = type;
	}

	void RenderCamera::SetMainViewMatrix(const Matrix4x4& viewMatrix, RenderCameraType type)
	{
		mCurrentCameraType = type;
		mViewMatrices[MAIN_VIEW_MATRIX_INDEX] = viewMatrix;

		Vector3 s = Vector3(viewMatrix[0][0], viewMatrix[0][1], viewMatrix[0][2]);
		Vector3 u = Vector3(viewMatrix[1][0], viewMatrix[1][1], viewMatrix[1][2]);
		Vector3 f = Vector3(-viewMatrix[2][0], -viewMatrix[2][1], -viewMatrix[2][2]);
		mPosition = s * (-viewMatrix[0][3]) + u * (-viewMatrix[1][3]) + f * viewMatrix[2][3];
	}

	void RenderCamera::Move(const Vector3& delta)
	{
		mPosition += delta;
	}

	void RenderCamera::Rotate(Vector2 delta)
	{
		delta = Vector2(Radian(Degree(delta[0])).ValueRadians(), Radian(Degree(delta[1])).ValueRadians());

		float dot = mUpAxis.DotProduct(Forward());
		if ((dot < -0.99f && delta[0] > 0.0f) || // angle nearing 180 degrees
			(dot > 0.99f && delta[0] < 0.0f))    // angle nearing 0 degrees
			delta[0] = 0.0f;

		Quaternion pitch, yaw;
		pitch.SetDataFromAngleAxis(Radian(delta[0]), Vector3::UNIT_X);
		yaw.SetDataFromAngleAxis(Radian(delta[0]), Vector3::UNIT_Z);

		mRotation = pitch * mRotation * yaw;

		mInvRotation = mRotation.Conjugate();
	}

	void RenderCamera::Zoom(float offset)
	{
		mFOVx = Math::Clamp(mFOVx - offset, MIN_FOV, MAX_FOV);
	}

	void RenderCamera::LookAt(const Vector3& position, const Vector3& target, const Vector3& up)
	{
		mPosition = position;

		Vector3 forward = (target - position).GetNormalised();
		mRotation = forward.GetRotationTo(Vector3::UNIT_Y);

		Vector3 right = forward.CrossProduct(up.GetNormalised()).GetNormalised();
		Vector3 orthUp = right.CrossProduct(forward);

		Quaternion upRotation = (mRotation * orthUp).GetRotationTo(Vector3::UNIT_Z);

		mRotation = Quaternion(upRotation) * mRotation;

		mInvRotation = mRotation.Conjugate();
	}

	void RenderCamera::SetAspect(float aspect)
	{
		mAspect = aspect;
		mFOVy = Radian(Math::Atan(Math::Tan(Radian(Degree(mFOVx) * 0.5f)) / mAspect) * 2.0f).ValueDegrees();
	}

	void RenderCamera::SetFOVx(float fovx)
	{
		mFOVx = fovx;
	}

	Vector3 RenderCamera::GetPosition() const
	{
		return mPosition;
	}

	Quaternion RenderCamera::GetRotation() const
	{
		return mRotation;
	}

	Vector3 RenderCamera::Forward() const
	{
		return mInvRotation * Vector3::UNIT_Y;
	}

	Vector3 RenderCamera::Up() const
	{
		return mInvRotation * Vector3::UNIT_Z;
	}

	Vector3 RenderCamera::Right() const
	{
		return mInvRotation * Vector3::UNIT_X;
	}

	Vector2 RenderCamera::GetFOV() const
	{
		return Vector2(mFOVx, mFOVy);
	}

	Matrix4x4 RenderCamera::GetViewMatrix()
	{
		Matrix4x4 viewMatrix = Matrix4x4::IDENTITY;
		switch (mCurrentCameraType)
		{
		case RenderCameraType::Editor:
			viewMatrix = Math::MakeLookAtMatrix(mPosition, mPosition + Forward(), Up());
			break;
		case RenderCameraType::Motor:
			viewMatrix = mViewMatrices[MAIN_VIEW_MATRIX_INDEX];
			break;
		default:
			break;
		}
		return viewMatrix;
	}

	Matrix4x4 RenderCamera::GetPersProjMatrix() const
	{
		return Math::MakePerspectiveMatrix(Radian(Degree(mFOVy)), mAspect, mZNear, mZFar);
	}

	Matrix4x4 RenderCamera::GetLookAtMatrix() const
	{
		return Math::MakeLookAtMatrix(mPosition, mPosition + Forward(), Up());
	}

	float RenderCamera::GetFOVyDeprecated() const
	{
		return mFOVy;
	}
}
