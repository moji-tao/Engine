#pragma once
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Quaternion.h"

namespace Engine
{
	struct Plane
	{
		Vector3 mNormal;
		Vector3 mPoint;

	public:
		float Distance(const Vector3& point) const;
	};

	struct BoundingBox
	{
		Vector3 mMin;
		Vector3 mMax;

	public:
		void Transform(BoundingBox& out, Matrix4x4& mat);
	};

	struct BoundingOrientedBox
	{
		Vector3 mCenter;
		Vector3 mExtents;
		Quaternion mOrientation;
	};

	struct BoundingSphere
	{
		Vector3 mCenter;
		float mRadius;
	};

	struct BoundingFrustum
	{
		/*
		Vector3 mOrigin;
		Quaternion mOrientation;

		float RightSlope;           // Positive X (X/Z)
		float LeftSlope;            // Negative X
		float TopSlope;             // Positive Y (Y/Z)
		float BottomSlope;          // Negative Y
		float Near, Far;            // Z of the near plane and far plane.
		*/
		Plane mPlane[6];

		Plane& mRight = mPlane[0];
		Plane& mLeft = mPlane[1];
		Plane& mTop = mPlane[2];
		Plane& mBottom = mPlane[3];
		Plane& mNear = mPlane[4];
		Plane& mFar = mPlane[5];
	public:
		enum ContainmentType
		{
			DISJOINT, // 不相交
			INTERSECTS, // 相切
			CONTAINS, // 包含
		};

		//void Transform(BoundingFrustum& out, const Matrix4x4& viewToLocalWorld);

		ContainmentType Contains(const BoundingBox& box) const;
		ContainmentType Contains(const BoundingOrientedBox& box) const;
		ContainmentType Contains(const BoundingSphere& sphere) const;
	};

	struct Ray
	{
		Vector3 mStart;
		Vector3 mDirectional;
	};

	class Collision
	{
	public:
		
	};
}
