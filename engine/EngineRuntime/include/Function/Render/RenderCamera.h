#pragma once
#include <vector>
#include "EngineRuntime/include/Core/Math/Matrix4x4.h"
#include "EngineRuntime/include/Core/Math/Quaternion.h"
#include "EngineRuntime/include/Core/Math/Vector2.h"
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Angle.h"

namespace Engine
{
	enum RenderCameraType : int
	{
		Editor,
		Motor
	};

	class RenderCamera
	{
	public:
		RenderCamera() = default;

		~RenderCamera() = default;

	public:
		void SetCurrentCameraType(RenderCameraType type);

		void SetMainViewMatrix(const Matrix4x4& viewMatrix, RenderCameraType type = RenderCameraType::Editor);

		void Move(const Vector3& delta);

		void Rotate(Vector2 delta);

		void Zoom(float offset);

		void LookAt(const Vector3& position, const Vector3& target, const Vector3& up);

		void SetAspect(float aspect);

		void SetFOVx(float fovx);

		Vector3 GetPosition() const;

		Quaternion GetRotation() const;

		Vector3 Forward() const;

		Vector3 Up() const;

		Vector3 Right() const;

		Vector2 GetFOV() const;

		Matrix4x4 GetViewMatrix();

		Matrix4x4 GetPersProjMatrix() const;

		Matrix4x4 GetLookAtMatrix() const;

		float GetFOVyDeprecated() const;

	private:
		RenderCameraType mCurrentCameraType = RenderCameraType::Editor;

		Vector3 mPosition = Vector3::ZERO;

		Quaternion mRotation = Quaternion::IDENTITY;
		Quaternion mInvRotation = Quaternion::IDENTITY;

	public:
		float mZNear = 1000.0f;
		float mZFar = 0.1f;

	private:
		Vector3 mUpAxis = Vector3::UNIT_Z;

		std::vector<Matrix4x4> mViewMatrices{ Matrix4x4::IDENTITY };

		float mAspect = 0.0f;

		float mFOVx = Degree(89.f).ValueDegrees();
		float mFOVy = 0.f;

	private:
		static constexpr float MIN_FOV{ 10.0f };
		static constexpr float MAX_FOV{ 89.0f };
		static constexpr int   MAIN_VIEW_MATRIX_INDEX{ 0 };
	};
}
