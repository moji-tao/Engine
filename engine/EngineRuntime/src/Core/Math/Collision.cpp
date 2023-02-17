#include "EngineRuntime/include/Core/Math/Collision.h"

namespace Engine
{
	/*
	void BoundingFrustum::Transform(BoundingFrustum& out, const Matrix4x4& viewToLocalWorld)
	{
		out.mOrientation = viewToLocalWorld.ExtractQuaternion() * out.mOrientation;
		out.mOrigin += viewToLocalWorld.GetTrans();

		out.RightSlope = RightSlope;
		out.LeftSlope = LeftSlope;
		out.TopSlope = TopSlope;
		out.BottomSlope = BottomSlope;
	}
	*/
	BoundingFrustum::ContainmentType BoundingFrustum::Contains(const BoundingBox& box) const
	{
		for (int i = 0; i < 6; ++i)
		{
			int k = 0;
			if (mPlane[i].Distance(box.mMin) < 0.0f)
			{
				++k;
			}
			if (mPlane[i].Distance(box.mMax) < 0.0f)
			{
				++k;
			}
			if (k == 2)
			{
				return ContainmentType::DISJOINT;
			}
		}
		return ContainmentType::CONTAINS;
	}

	BoundingFrustum::ContainmentType BoundingFrustum::Contains(const BoundingOrientedBox& box) const
	{
		return ContainmentType::CONTAINS;
	}

	BoundingFrustum::ContainmentType BoundingFrustum::Contains(const BoundingSphere& sphere) const
	{
		return ContainmentType::CONTAINS;
	}

	float Plane::Distance(const Vector3& point) const
	{
		return mNormal.DotProduct(point - mPoint);
	}

	void BoundingBox::Transform(BoundingBox& out, Matrix4x4& mat)
	{
		Vector4 minPoint(mMin, 1.0f);
		Vector4 maxPoint(mMax, 1.0f);

		minPoint = minPoint * mat;
		maxPoint = maxPoint * mat;
		minPoint /= minPoint.w;
		maxPoint /= maxPoint.w;

		out.mMax = Vector3(maxPoint);
		out.mMin = Vector3(minPoint);
	}
}
