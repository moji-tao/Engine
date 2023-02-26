#include <fstream>
#include <yaml-cpp/yaml.h>
#include "EngineRuntime/include/Core/Math/Math.h"
#include "EngineRuntime/include/Core/Math/Matrix4x4.h"
#include "EngineRuntime/include/Core/Math/Transform.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/AnimationClip.h"

namespace Engine
{
    IMPLEMENT_RTTI(AnimationClip, Object);

    AnimationClip::AnimationClip()
	    :mAnimationLength(0.0), mTicksPerSecond(0.0)
    { }

	AnimationClip::~AnimationClip()
    { }

	bool AnimationClip::GetBoneKeyMat(const std::string& boneName, double second, Matrix4x4& mat)
    {
        ASSERT(second >= 0.0);

        double keyFrame = second * mTicksPerSecond;

        while (keyFrame >= mAnimationLength)
        {
            keyFrame -= mAnimationLength;
        }

        auto it = mAnimation.find(boneName);

        if (it == mAnimation.end())
        {
            mat = Matrix4x4::IDENTITY;
            return false;
        }

        std::map<double, Transform>& bonePoseKeys = it->second;

        GetKeyFrameLerp(keyFrame, bonePoseKeys, mat);

        return true;
    }

    bool AnimationClip::GetBoneKeyTransform(const std::string& boneName, double second, Transform& transform)
    {
        ASSERT(second >= 0.0);

        double keyFrame = second * mTicksPerSecond;

        while (keyFrame >= mAnimationLength)
        {
            keyFrame -= mAnimationLength;
        }

        auto it = mAnimation.find(boneName);

        if (it == mAnimation.end())
        {
            return false;
        }

        std::map<double, Transform>& bonePoseKeys = it->second;

        GetKeyFrameLerp(keyFrame, bonePoseKeys, transform);

        return true;
    }

    void AnimationClip::AddBoneKey(const std::string& boneName, double keyTime, const Transform& transform)
    {
	    
        mAnimation[boneName][keyTime] = transform;
    }

    void AnimationClip::SetAnimationTime(double time)
    {
        mAnimationLength = time;
    }

    void AnimationClip::SetTicksPerSecond(double tc)
    {
        mTicksPerSecond = tc;
    }

    double AnimationClip::GetAnimationTime()
    {
        return mAnimationLength / mTicksPerSecond;
    }

    double AnimationClip::GetTicksPerSecond()
    {
        return mTicksPerSecond;
    }

    std::string AnimationClip::GetAnimationClipName()
    {
        return mName;
    }

    void AnimationClip::SetAnimationClipName(const std::string& name)
    {
        mName = name;
    }

    void AnimationClip::GetKeyFrameLerp(double keyFrameTime, const std::map<double, Transform>& keyFrameMap, Matrix4x4& mat)
    {
        auto bg = keyFrameMap.lower_bound(keyFrameTime);

        if (bg == keyFrameMap.end())
        {
            // 传入key超出了范围
            //LOG_WARN("时间帧超出范围 {0}", keyFrameTime);
            --bg;
            mat = bg->second.getMatrix();
            return;
        }

        if (keyFrameTime == bg->first)
        {
            // 时间刚好等于关键帧 返回it->second
            mat = bg->second.getMatrix();
            return;
        }

        if (bg == keyFrameMap.begin())
        {
            // key 小于第一个 返回单位矩阵
            mat = Matrix4x4::IDENTITY;
            return;
        }

        auto ed = bg--;

        double t = (keyFrameTime - bg->first) / (ed->first - bg->first);

        Vector3 position = Math::Lerp(bg->second.position, ed->second.position, t);
        Vector3 scale = Math::Lerp(bg->second.scale, ed->second.scale, t);
        Quaternion rotation = Quaternion::nLerp(t, bg->second.rotation, ed->second.rotation, true);

        mat.MakeTransform(position, scale, rotation);
    }

    void AnimationClip::GetKeyFrameLerp(double keyFrameTime, const std::map<double, Transform>& keyFrameMap, Transform& transform)
    {
        auto bg = keyFrameMap.lower_bound(keyFrameTime);

        if (bg == keyFrameMap.end())
        {
            // 传入key超出了范围
            //LOG_WARN("时间帧超出范围 {0}", keyFrameTime);
            --bg;
            transform = bg->second;
            return;
        }

        if (keyFrameTime == bg->first)
        {
            // 时间刚好等于关键帧 返回it->second
            transform = bg->second;
            return;
        }

        if (bg == keyFrameMap.begin())
        {
            // key 小于第一个 返回单位矩阵
            transform.position = Vector3::ZERO;
            transform.scale = Vector3::UNIT_SCALE;
            transform.rotation = Quaternion::IDENTITY;
            return;
        }

        auto ed = bg--;

        double t = (keyFrameTime - bg->first) / (ed->first - bg->first);

        transform.position = Math::Lerp(bg->second.position, ed->second.position, t);
        transform.scale = Math::Lerp(bg->second.scale, ed->second.scale, t);
        transform.rotation = Quaternion::nLerp(t, bg->second.rotation, ed->second.rotation, true);

    }

	void AnimationClip::Serialize(SerializerDataFrame& stream) const
    {
        stream << mName;

        stream << mAnimationLength;

        stream << mTicksPerSecond;

        stream << mAnimation;
    }

	bool AnimationClip::Deserialize(SerializerDataFrame& stream)
    {
        stream >> mName;

        stream >> mAnimationLength;

        stream >> mTicksPerSecond;

        stream >> mAnimation;

        return true;
    }

    void AnimationClipMeta::Load(const std::filesystem::path& metaPath)
    {
        YAML::Node node = YAML::LoadFile(ProjectFileSystem::GetInstance()->GetActualPath(metaPath).generic_string());

        ASSERT(node["guid"].IsDefined());

        if (mGuid != nullptr)
        {
            delete mGuid;
        }

        mGuid = new GUID(node["guid"].as<std::string>());
    }

    void AnimationClipMeta::Save(const std::filesystem::path& metaPath)
    {
        ASSERT(mGuid != nullptr);

        YAML::Node node;
        node["guid"] = mGuid->Data();

        std::ofstream fout(ProjectFileSystem::GetInstance()->GetActualPath(metaPath));
        fout << node;
    }
}
