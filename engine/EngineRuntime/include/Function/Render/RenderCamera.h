#pragma once
#include <vector>
#include "EngineRuntime/include/Core/Math/Matrix4x4.h"
#include "EngineRuntime/include/Core/Math/Quaternion.h"
#include "EngineRuntime/include/Core/Math/Vector2.h"
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Angle.h"
#include "EngineRuntime/include/Core/Math/Collision.h"

namespace Engine
{
	enum RenderCameraType : int
	{
		Editor,
		Motor
	};

	// Z向前 Y向上 X向右

	class RenderCamera
	{
	public:
		RenderCamera() = default;

		~RenderCamera() = default;

	public:
		void SetCurrentCameraType(RenderCameraType type);

		void SetMainViewMatrix(const Matrix4x4& viewMatrix, RenderCameraType type = RenderCameraType::Editor);

		//void Move(const Vector3& delta);

		void MoveForward(float dist);

		void MoveUp(float dist);

		void MoveRight(float dist);

		//void Rotate(Vector2 delta);
		// 鼠标左右动
		void Pitch(Degree degrees); //
		// 鼠标上下动
		void Yaw(Degree degrees); //

		void Zoom(float offset);

		void LookAt(const Vector3& position, const Vector3& target, const Vector3& up); //

		void SetRenderSize(int width, int height);

		void SetAspect(float aspect);

		void SetFOVx(float fovx);

		Vector2 GetRenderSize() const;

		Vector3 GetPosition() const;

		//Quaternion GetRotation() const;

		Vector3 Forward() const;

		Vector3 Up() const;

		Vector3 Right() const;

		Vector2 GetFOV() const;

		Matrix4x4 GetViewMatrix() const;

		Matrix4x4 GetPersProjMatrix() const;

		Matrix4x4 GetLookAtMatrix() const;

		float GetFOVyDeprecated() const;

	private:
		RenderCameraType mCurrentCameraType = RenderCameraType::Editor;

		Vector3 mPosition = Vector3::ZERO;

		Vector3 mForward = Vector3::UNIT_Z;
		Vector3 mUp = Vector3::UNIT_Y;
		Vector3 mRight = Vector3::UNIT_X;

		//Quaternion mRotation = Quaternion::IDENTITY;
		//Quaternion mInvRotation = Quaternion::IDENTITY;

	public:
		float mZNear = 0.03f;
		float mZFar = 10000.0f;

	private:
		//Vector3 mUpAxis = Vector3::UNIT_Z;

		std::vector<Matrix4x4> mViewMatrices{ Matrix4x4::IDENTITY };

		float mAspect = 0.0f;

		float mFOVx = Degree(45.f).ValueDegrees();
		float mFOVy = 0.f;

		int mRenderWidth = 1920;
		int mRenderHeight = 1080;

	private:
		float mTotalTime = 0.0f;
		float mDeltaTime = 0.0f;

	private:
		static constexpr float MIN_FOV{ 10.0f };
		static constexpr float MAX_FOV{ 89.0f };
		static constexpr int   MAIN_VIEW_MATRIX_INDEX{ 0 };
	};
}
